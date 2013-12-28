#pragma once
#include "at_exit.h"
#include "AutoFactory.h"
#include "AutoPacketSubscriber.h"
#include "autowiring_error.h"
#include "Bolt.h"
#include "CoreThread.h"
#include "CurrentContextPusher.h"
#include "DeferredBase.h"
#include "ExceptionFilter.h"
#include "JunctionBox.h"
#include "JunctionBoxManager.h"
#include "PolymorphicTypeForest.h"
#include "SimpleOwnershipValidator.h"
#include "TeardownNotifier.h"
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>
#include <list>
#include <memory>
#include <map>
#include <string>
#include TYPE_INDEX_HEADER
#include FUNCTIONAL_HEADER
#include EXCEPTION_PTR_HEADER
#include SHARED_PTR_HEADER
#include STL_UNORDERED_MAP


#ifndef ASSERT
  #ifdef _DEBUG
    #include <assert.h>
    #define ASSERT(x) assert(x)
  #else
    #define ASSERT(x)
  #endif
#endif

class AutoPacketFactory;
class AutowirableSlot;
class BoltBase;
class ContextMember;
class CoreContext;
class CoreThread;
class EventReceiver;
class GlobalCoreContext;
class OutstandingCountTracker;

template<class T>
class Autowired;

template<class Sigil1, class Sigil2, class Sigil3>
struct Boltable;

/// <summary>
/// This class is used to determine whether all core threads have exited
/// </summary>
class CoreContext:
  public Object,
  public SimpleOwnershipValidator,
  public TeardownNotifier,
  public std::enable_shared_from_this<CoreContext>
{
protected:
  CoreContext(std::shared_ptr<CoreContext> pParent, const std::type_info& sigil);
  CoreContext(std::shared_ptr<CoreContext> pParent, const std::type_info& sigil, std::shared_ptr<CoreContext> pPeer);

  // A pointer to the current context, for construction purposes
  static boost::thread_specific_ptr<std::shared_ptr<CoreContext> > s_curContext;

public:
  virtual ~CoreContext(void);

  /// <summary>
  /// Factory to create a new context
  /// </summary>
  /// <param name="T">The context sigil.  If void, identical to CreateAnonymous.</param>
  template<class T>
  std::shared_ptr<CoreContext> Create(void) {
    return Create(typeid(T));
  }

  /// <summary>
  /// Factory to create an anonymous context
  /// </summary>
  std::shared_ptr<CoreContext> CreateAnonymous(void) {
    return Create(typeid(void));
  }

  /// <summary>
  /// Factory to create a peer context
  /// </summary>
  /// <remarks>
  /// A peer context allows clients to create autowiring contexts which are in the same event
  /// domain with respect to each other, but are not in the same autowiring domain.  This can
  /// be useful where multiple instances of a particular object are desired, but inserting
  /// such objects into a simple child context is cumbersome because the objects at parent
  /// scope are listening to events originating from objects at child scope.
  /// </remarks>
  template<class T>
  std::shared_ptr<CoreContext> CreatePeer(void) {
    return CreatePeer(typeid(T));
  }

  /// <summary>
  /// Factory to create an anonymous peer context
  /// </summary>
  template<class T>
  std::shared_ptr<CoreContext> CreateAnonymousPeer(void) {
    return CreatePeer(typeid(void));
  }

  /// <summary>
  /// Allows a specifically named class to be bolted
  /// </summary>
  /// <remarks>
  /// If the specified type does not inherit from BoltTo, this method has no effect
  /// </remarks>
  template<class T>
  void Enable(void) {
    static_assert(!std::is_abstract<T>::value, "Cannot enable an abstract class for bolting");
    EnableInternal((T*)nullptr, (T*)nullptr);
  }

  /// <summary>
  /// Convenience method to obtain a shared reference to the global context
  /// </summary>
  static std::shared_ptr<CoreContext> GetGlobal(void);

protected:
  std::shared_ptr<CoreContext> Create(const std::type_info& sigil);
  std::shared_ptr<CoreContext> Create(const std::type_info& sigil, CoreContext& newContext);
  std::shared_ptr<CoreContext> CreatePeer(const std::type_info& sigil);

  // General purpose lock for this class
  mutable boost::mutex m_lock;

  // The context's internally held sigil type
  const std::type_info& m_sigil;

  // The context's internally held full-path name (recursively defined through parents)--required to be a literal string
  std::string m_fullPathName;
  
    // Flag, set if this context should use its ownership validator to guarantee that all autowired members
  // are correctly torn down.  This flag must be set at construction time.  Members added to the context
  // before this flag is assigned will NOT be checked.
  bool m_useOwnershipValidator;

  // A pointer to the parent context
  std::shared_ptr<CoreContext> m_pParent;

  // This is a map of the context members by type and, where appropriate, by name
  // This map keeps all of its objects resident at least until the context goes away.
  // "Object" is named here as an explicit ground type in order to allow arbitrary casting from Object-
  // derived types.
  PolymorphicTypeForest<ExplicitGrounds<Object>> m_byType;

  // All ContextMember objects known in this autowirer:
  std::unordered_set<ContextMember*> m_contextMembers;

  // Collection of objects waiting to be autowired, and a specific lock exclusively for this collection
  boost::mutex m_deferredLock;
  typedef std::map<const AutowirableSlot*, DeferredBase*> t_deferred;
  t_deferred m_deferred;

  // All known event receivers and receiver proxies originating from this context:
  typedef std::unordered_set<std::shared_ptr<EventReceiver>, SharedPtrHash<EventReceiver>> t_rcvrSet;
  t_rcvrSet m_eventReceivers;

  //TODO: This is the new type when JunctionBoxManager is done
  typedef std::shared_ptr<JunctionBoxManager> t_junctionBoxes;
  //typedef std::unordered_map<std::type_index, std::shared_ptr<JunctionBoxBase>> t_junctionBoxes;
  t_junctionBoxes m_junctionBoxes;
  

  // All known snoopers.  Snoopers will not be removed from parent scopes on destruction.
  t_rcvrSet m_snoopers;

  // All known exception filters:
  std::unordered_set<ExceptionFilter*> m_filters;

  // The current RUN count.  This is the number of times that InitiateCoreThreads has been called.
  // SignalShutdown must be called an equal number of times to actually shut down this context.
  int m_refCount;

  // This is a global STOP variable, used to signal shutdown when it's time to quit.
  bool m_shouldStop;

  // Condition, signalled when context state has been changed
  boost::condition m_stateChanged;

  // Clever use of shared pointer to expose the number of outstanding CoreThread instances.
  // Destructor does nothing; this is by design.
  std::weak_ptr<Object> m_outstanding;

  // Actual core threads:
  typedef std::list<CoreThread*> t_threadList;
  t_threadList m_threads;

  // Child contexts:
  typedef std::list<std::weak_ptr<CoreContext> > t_childList;
  boost::mutex m_childrenLock;
  t_childList m_children;

  friend std::shared_ptr<GlobalCoreContext> GetGlobalContext(void);

  // The interior packet factory:
  std::shared_ptr<AutoPacketFactory> m_packetFactory;

  // Lists of event receivers, by name:
  typedef std::unordered_map<std::type_index, std::list<BoltBase*>> t_contextNameListeners;
  t_contextNameListeners m_nameListeners;

  // Adds a bolt proper to this context
  template<class T, class Sigil>
  void EnableInternal(T*, Bolt<Sigil>*) {
    std::shared_ptr<T> ptr;
    AutoRequire(ptr);
  }

  template<class Sigil, class T>
  void AutoRequireMicroBolt(void);

  // Enables a boltable class
  template<class T, class Sigil1, class Sigil2, class Sigil3>
  void EnableInternal(T*, Boltable<Sigil1, Sigil2, Sigil3>*) {
    AutoRequireMicroBolt<Sigil1, T>();
    AutoRequireMicroBolt<Sigil2, T>();
    AutoRequireMicroBolt<Sigil3, T>();
  }

  void EnableInternal(...) {}

  /// <summary>
  /// Broadcasts a notice to any listener in the current context regarding a creation event on a particular context name
  /// </summary>
  /// <remarks>
  /// The broadcast is made without altering the current context.  Recipients expect that the current context will be the
  /// one about which they are being informed.
  /// </remarks>
  void BroadcastContextCreationNotice(const std::type_info& sigil) const;

  /// <summary>
  /// Invokes all deferred autowiring fields, generally called after a new member has been added
  /// </summary>
  void UpdateDeferredElements(void);

  /// <summary>
  /// Adds the named event receiver to the collection of known receivers
  /// </summary>
  void AddEventReceiver(std::shared_ptr<EventReceiver> pRecvr);

  /// <summary>
  /// Removes the named event receiver from the collection of known receivers
  /// </summary>
  void RemoveEventReceiver(std::shared_ptr<EventReceiver> pRecvr);

  /// <summary>
  /// Removes all recognized event receivers in the indicated range
  /// </summary>
  void RemoveEventReceivers(t_rcvrSet::iterator first, t_rcvrSet::iterator last);

  /// <summary>
  /// Adds an object of any kind to the IOC container
  /// </summary>
  /// <param name="pContextMember">The member which was added</param>
  /// <param name="notReady">Allows the insertion of a thread, even if that thread isn't ready yet</param>
  /// <return>The shared pointer which contains the context member.</return>
  /// <remarks>
  /// It's safe to allow the returned shared_ptr to go out of scope; the core context
  /// will continue to hold a reference to it until Remove is invoked.
  /// </remarks>
  void AddCoreThread(const std::shared_ptr<CoreThread>& pCoreThread);

  /// <summary>
  /// Adds the specified context creation listener to receive creation events broadcast from this context
  /// </summary>
  /// <param name="pBase">The instance being added</param>
  void AddBolt(const std::shared_ptr<BoltBase>& pBase);

  /// <summary>
  /// Overload of Add based on ContextMember
  /// </summary>
  void AddContextMember(const std::shared_ptr<ContextMember>& ptr);

  /// <summary>
  /// Forwarding routine, adds a packet subscriber to the internal packet factory
  /// </summary>
  void AddPacketSubscriber(AutoPacketSubscriber&& rhs);

  /// <summary>
  /// Default override, when a member does not have an autofilter routine
  /// </summary>
  void AddPacketSubscriber(const std::false_type&) {}

  /// <summary>
  /// Identical to Autowire, but will not register the passed slot for deferred resolution
  /// </summary>
  template<class W>
  bool AutowireNoDefer(W& slot) {
    typedef typename W::element_type T;

    // First-chance resolution in this context and ancestor contexts:
    for(CoreContext* pCur = this; pCur; pCur = pCur->m_pParent.get()) {
      pCur->FindByType(slot);
      if(slot)
        return true;
    }

    if(
      has_static_new<T>::value ||
      has_simple_constructor<T>::value
    )
      // We will not attempt second-chance resolution if the type is constructable
      return false;

    // Attempt second-chance resolution:
    std::shared_ptr<AutoFactory<T>> factory;
    for(CoreContext* pCur = this; pCur; pCur = pCur->m_pParent.get()) {
      pCur->FindByType(factory);
      if(!factory)
        continue;

      std::shared_ptr<T> ptr(factory->New());
      Add(ptr);
      slot.swap(ptr);
      return true;
    }

    return false;
  }

  /// <summary>
  /// Increments the total number of contexts still outstanding
  /// </summary>
  /// <remarks>
  /// This is an indirect incrementation routine.  The count will be incremented for as
  /// long as the returned shared_ptr is not destroyed.  Once it's destroyed, the count
  /// is decremented.  The caller is encouraged not to copy the return value, as doing
  /// so can give spurious values for the current number of outstanding threads.
  ///
  /// The caller is responsible for exterior synchronization
  /// </remarks>
  std::shared_ptr<Object> IncrementOutstandingThreadCount(void);

public:
  // Accessor methods:
  bool IsGlobalContext(void) const { return !m_pParent; }
  size_t GetMemberCount(void) const {return m_byType.size();}
  bool IsRunning(void) const {return !!m_refCount;}
  const std::type_info& GetSigilType(void) const { return m_sigil; }

  /// <summary>
  /// In debug mode, adds an additional compile-time check
  /// </summary>
  /// <remarks>
  /// Enabling simple ownership checks on a context will ensure that, at teardown time, simple
  /// ownership of contained objects is enforced.  This means that the lifetime of objects in
  /// a context does not extend beyond the lifetime of the context itself.
  ///
  /// This flag is useful in detecting cycles in a context, as cycles will prevent cleanup and
  /// give the impression of an exterior reference.  Information about the detected cycle will
  /// be printed to stderr.
  ///
  /// This flag cannot be unset.  This flag is inherited by children.  Children which were
  /// created at the time this flag is assigned do not receive assignment of this flag retro-
  /// actively.
  /// </remarks>
  void EnforceSimpleOwnership(void) {
    m_useOwnershipValidator = true;
  }

  /// <returns>
  /// True if CoreThread instances in this context should begin teardown operations
  /// </returns>
  bool ShouldStop(void) const {return m_shouldStop;}

  /// <returns>
  /// True if this context was ever started
  /// </returns>
  /// <remarks>
  /// A return value of "true" is guaranteed to be indefinitely correct.  A return value of
  /// "false" will only be correct for as long as it takes for someone to start this context.
  /// Unless externally synchronized, this operation may return false on a running context.
  /// </remarks>
  bool WasStarted(void) const {
    // We were started IF we are currently running, OR we have been signalled to stop
    return IsRunning() || ShouldStop();
  }

  /// <returns>
  /// True if this context is an ancestor of the specified context
  /// </returns>
  /// <remarks>
  /// This method will also return true if this == child
  /// </remarks>
  bool IsAncestorOf(const CoreContext* child) const {
    for(auto cur = child; cur; cur = cur->GetParentContext().get())
      if(cur == this)
        return true;
    return false;
  }

  /// <summary>
  /// Determines whether the passed type is a member of this context, or any ancestor context
  /// </summary>
  template<class T>
  bool IsMember(typename std::enable_if<std::is_base_of<ContextMember, T>::value, T*>::type ptr) const {
    return
      static_cast<ContextMember*>(ptr)->GetContext().get() == this;
  }
  
  template<class T>
  bool IsMember(typename std::enable_if<!std::is_base_of<ContextMember, T>::value, T*>::type ptr) const {
    // If the passed type is a ContextMember, we can query relationship status
    ContextMember* pMember = dynamic_cast<ContextMember*>(ptr);
    return
      pMember ?
      pMember->GetContext().get() == this :
      m_byType.Contains<T>();
  }
  
  template<class T>
  bool IsMember(const std::shared_ptr<T>& ptr) const {
    return IsMember<T>(ptr.get());
  }

  /// <summary>
  /// Obtains a shared pointer to an event sender _in this context_ matching the specified type
  /// </summary>
  template<class T>
  std::shared_ptr<JunctionBox<T>> GetJunctionBox(void) {
    /*
    std::shared_ptr<JunctionBox<T>> retVal;
    boost::lock_guard<boost::mutex> lk(m_lock);
    auto q = m_junctionBoxes.find(typeid(T));
    if(q != m_junctionBoxes.end())
      // No dynamic cast is needed here, we already have independent knowledge of the
      // destination type because it's the key type of our map
      return std::static_pointer_cast<JunctionBox<T>, JunctionBoxBase>(q->second);

    // Construct new type:
    retVal.reset(new JunctionBox<T>);
    m_junctionBoxes[typeid(T)] = retVal;
    
    // Attach compatible receivers:
    for(auto q = m_eventReceivers.begin(); q != m_eventReceivers.end(); q++)
      *retVal += *q;

    // Construction complete
    return retVal;
     */
    
    //TODO: Replace above with below when JunctionBoxManager is complete
    return std::static_pointer_cast<JunctionBox<T>, JunctionBoxBase>(m_junctionBoxes->Get(typeid(T)));
  }

  /// <summary>
  /// Adds an object of any kind to the IOC container
  /// </summary>
  /// <param name="T">The concrete type to be added</param>
  /// <param name="value">The member to add</param>
  /// <remarks>
  /// It's safe to allow the returned shared_ptr to go out of scope; the core context
  /// will continue to hold a reference to it until Remove is invoked.
  /// </remarks>
  template<class T>
  void Add(const std::shared_ptr<T>& value) {
    // Extract ground for this value, we'll use it to select the correct forest for the value:
    typedef typename ground_type_of<T>::type groundType;

    // Validate that this addition does not generate an ambiguity:
    {
      std::shared_ptr<T> ptr;
      FindByType<T>(ptr);
      if(ptr == value)
        throw std::runtime_error("An attempt was made to add the same value to the same context more than once");
      if(ptr)
        throw std::runtime_error("An attempt was made to add the same type to the same context more than once");
    }

    {
      boost::lock_guard<boost::mutex> lk(m_lock);

      // Add a new member of the forest:
      m_byType.AddTree(value);

      // Context members:
      auto pContextMember = std::fast_pointer_cast<ContextMember, T>(value);
      if(pContextMember) {
        AddContextMember(pContextMember);

        // CoreThreads:
        auto pCoreThread = std::fast_pointer_cast<CoreThread, T>(value);
        if(pCoreThread)
          AddCoreThread(pCoreThread);
      }
      
      // Exception filters:
      auto pFilter = std::fast_pointer_cast<ExceptionFilter, T>(value);
      if(pFilter)
        m_filters.insert(pFilter.get());
      
      // Bolts:
      auto pBase = std::fast_pointer_cast<BoltBase, T>(value);
      if(pBase)
        AddBolt(pBase);
    }
    
    // Event receivers:
    auto pRecvr = std::fast_pointer_cast<EventReceiver, T>(value);
    if(pRecvr)
      AddEventReceiver(pRecvr);

    // Subscribers:
    AddPacketSubscriber(AutoPacketSubscriberSelect<T>(value));

    // Notify any autowiring field that is currently waiting that we have a new member
    // to be considered.
    UpdateDeferredElements();

    // Ownership validation, as appropriate:
    if(m_useOwnershipValidator)
      SimpleOwnershipValidator::PendValidation(std::weak_ptr<T>(value));
  }

  /// <summary>
  /// Similar to the alternative overloaded Add method, except makes this context current before construction
  /// and creates a new shared_ptr for the constructed object
  /// </summary>
  template<class T>
  std::shared_ptr<T> Add(void) {
    CurrentContextPusher pusher(this);
    std::shared_ptr<T> retVal(new T);
    Add(retVal);
    return retVal;
  }

  /// <summary>
  /// Utility routine, invoked typically by the service, which starts all registered
  /// core threads.
  /// </summary>
  void InitiateCoreThreads(void);

  /// <summary>
  /// This signals to the whole system that a shutdown operation is underway, and that
  /// shutdown procedures should begin immediately
  /// </summary>
  void SignalShutdown(void);

  /// <summary>
  /// This terminates this context and all of its children, by force if necessary
  /// </summary>
  /// <param name="wait">Set if the function should wait for all child contexts to exit before returning</param>
  void SignalTerminate(bool wait = true);

  /// <summary>
  /// Waits for all threads holding references to exit
  /// </summary>
  void Wait(void) {
    boost::unique_lock<boost::mutex> lk(m_lock);
    m_stateChanged.wait(lk, [this] () {return this->m_outstanding.expired();});
  }

  template<class Rep, class Period>
  bool Wait(const boost::chrono::duration<Rep, Period>& duration) {
    boost::unique_lock<boost::mutex> lk(m_lock);
    return m_stateChanged.wait_for(lk, duration, [this] () {return this->m_outstanding.expired();});
  }

  /// <summary>
  /// This makes this core context current.
  /// </summary>
  /// <returns>The previously current context</returns>
  std::shared_ptr<CoreContext> SetCurrent(void) {
    std::shared_ptr<CoreContext> newCurrent = shared_from_this();
    ASSERT(newCurrent);
    return SetCurrent(newCurrent);
  }

  /// <summary>
  /// This makes a specific core context current
  /// </summary>
  /// <returns>The previously current context</returns>
  static std::shared_ptr<CoreContext> SetCurrent(const std::shared_ptr<CoreContext>& context) {
    ASSERT(context);
    std::shared_ptr<CoreContext> retVal = CurrentContext();
    s_curContext.reset(new std::shared_ptr<CoreContext>(context));
    return retVal;
  };

  /// <summary>
  /// Makes no context current
  /// </summary>
  /// <remarks>
  /// Generally speaking, users wishing to release their reference to some context can do so simply
  /// by making the global context current.  The sole exception is when the global context is being
  /// destroyed, in which case, a null context is exactly what's desired.
  /// </remarks>
  static void EvictCurrent(void) {
    s_curContext.reset();
  }

  /// <summary>
  /// This retrieves a shared pointer to the current context.  It is only contextually relevant.
  /// </summary>
  /// <return>
  /// The last core context to have called SetCurrent in the current thread, or else an empty pointer
  /// </return>
  /// <remarks>
  /// This works by using thread-local store, and so is safe in multithreaded systems.  The current
  /// context is assigned before invoking a CoreThread instance's Run method, and it's also assigned
  /// when a context is first constructed by a thread.
  /// </remarks>
  static std::shared_ptr<CoreContext> CurrentContext(void);

  /// <summary>
  /// Obtains a pointer to the parent context
  /// </summary>
  std::shared_ptr<CoreContext>& GetParentContext(void) {return m_pParent;}
  const std::shared_ptr<CoreContext>& GetParentContext(void) const {return m_pParent;}

  /// <summary>
  /// Filters std::current_exception using any registered exception filters, or rethrows.
  /// </summary>
  /// <remarks>
  /// The passed exception is assumed to be a generic exception whose default behavior
  /// shall be to tear down the context.  It will be the caller's responsibility to ensure
  /// that this behavior is observed.
  ///
  /// If the exception is successfully handled by a filter, this method returns cleanly.
  /// Otherwise, this method is equivalent to std::rethrow_exception.
  /// </remarks>
  void FilterException(void);

  /// <summary>
  /// Filters a std::current_exception thrown by an EventSenderBase during a Fire
  /// </summary>
  /// <param name="pProxy">The sender of the event</param>
  /// <param name="pRecipient">The recipient of the event</param>
  void FilterFiringException(const JunctionBoxBase* pProxy, EventReceiver* pRecipient);

  /// <summary>
  /// Enables the passed event receiver to obtain messages broadcast by this context
  /// </summary>
  /// <remarks>
  /// This enables the passed event receiver to snoop events that are broadcast from a
  /// parent context.  The passed event receiver MUST exist in a parent context, or the
  /// behavior of this method may be undefined during teardown.
  ///
  /// The snooper will not receive any events broadcast from parent contexts.  ONLY events
  /// broadcast in THIS context will be forwarded to the snooper.
  /// </remarks>
  template<class T>
  void Snoop(const std::shared_ptr<T>& pSnooper) {
    static_assert(std::is_base_of<EventReceiver, T>::value, "Cannot snoop on a type which is not an event receiver");

    // Snooping now
    auto rcvr = std::static_pointer_cast<EventReceiver, T>(pSnooper);
    (boost::lock_guard<boost::mutex>)m_lock,
    m_snoopers.insert(rcvr);

    // Pass control to the event adder helper:
    AddEventReceiver(rcvr);
  }

  /// <summary>
  /// Unregisters an event receiver previously registered to receive snooped events
  /// </summary>
  /// <remarks>
  /// It is an error to call this method without a prior call to Snoop
  /// </remarks>
  template<class T>
  void Unsnoop(const std::shared_ptr<T>& pSnooper) {
    static_assert(std::is_base_of<EventReceiver, T>::value, "Cannot unsnoop on a type which is not an event receiver");

    // Pass control to the event remover helper:
    auto rcvr = std::static_pointer_cast<EventReceiver, T>(pSnooper);
    (boost::lock_guard<boost::mutex>)m_lock,
    m_snoopers.erase(rcvr);
    RemoveEventReceiver(rcvr);
  }

  /// <summary>
  /// Locates an available context member in this context
  /// </summary>
  template<class T>
  void FindByType(std::shared_ptr<T>& slot) {
    boost::lock_guard<boost::mutex> lk(m_lock);
    if(!m_byType.Resolve(slot))
      throw_rethrowable autowiring_error("An autowiring operation resulted in an ambiguous match");
  }

  // Interior type overrides:
  void FindByType(std::shared_ptr<AutoPacketFactory>& slot) { slot = m_packetFactory; }

  template<class W>
  void AutoRequire(W& slot) {
    if(AutowireNoDefer(slot))
      return;

    // Failed, create
    std::shared_ptr<typename W::element_type> ptr(CreationRules::New<typename W::element_type>());
    Add(ptr);
    slot = ptr;
  }

  /// <summary>
  /// Registers a slot to be autowired
  /// </summary>
  template<class W>
  bool Autowire(W& slot) {
    if(AutowireNoDefer(slot))
      return true;

    // Failed, defer
    Defer(slot);
    return false;
  }

  template<class S>
  void Defer(S& slot) {
    class Deferred:
      public DeferredBase {
    public:
      Deferred(CoreContext* pThis, S& slot):
        DeferredBase(pThis, slot.m_tracker),
        slot(slot)
      {}

      S& slot;

      bool operator()() override {
        return
          this->tracker.expired() ||
          this->slot ||
          this->pThis->AutowireNoDefer(this->slot);
      }
    };

    // Resolution failed, add this autowired value for a delayed attempt
    boost::lock_guard<boost::mutex> lk(m_deferredLock);
    if(slot)
      // Someone autowired this before we did, short-circuit
      return;

    DeferredBase*& pDeferred = m_deferred[&slot];
    if(pDeferred) {
      // We allow rebinding at this site if the deferred base has already expired
      if(pDeferred->IsExpired())
        delete pDeferred;
      else
        throw_rethrowable autowiring_error("A slot is being autowired, but a deferred instance already exists at this location");
    }
    pDeferred = new Deferred(this, slot);
  }

  /// <summary>
  /// Adds a post-attachment listener in this context for a particular autowired member
  /// </summary>
  /// <remarks>
  /// This method will succeed if slot was constructed in this context or any parent context.  If the
  /// passed slot was not created in this context or a parent context, an exception will be thrown.
  ///
  /// It's possible that the passed slot will never be filled, and instead the corresponding instance
  /// destroyed without ever having been initialized.
  ///
  /// If the passed slot is already autowired, then the listener will be invoked immediately from the
  /// body of this method.  Care should be taken to avoid deadlocks in this case--either the caller must
  /// not be holding any locks when this method is invoked, or the caller should design the listener
  /// method such that it may be substitutde in place for the notification routine.
  /// </remarks>
  void NotifyWhenAutowired(const AutowirableSlot& slot, const std::function<void()>& listener);

  /// <summary>
  /// Utility debug method for writing a snapshot of this context to the specified output stream
  /// </summary>
  void Dump(std::ostream& os) const;

  /// <summary>
  /// Utility routine to print information about the current exception
  /// </summary>
  static void DebugPrintCurrentExceptionInformation();
};

std::ostream& operator<<(std::ostream& os, const CoreContext& context);

#include "MicroBolt.h"

template<class Sigil, class T>
void CoreContext::AutoRequireMicroBolt(void) {
  if(std::is_same<void, Sigil>::value)
    return;

  std::shared_ptr<MicroBolt<Sigil, T>> ptr;
  AutoRequire(ptr);
}

