#pragma once
#include "at_exit.h"
#include "AutoAnchor.h"
#include "AutoFactory.h"
#include "AutoPacketSubscriber.h"
#include "AutowiringEvents.h"
#include "autowiring_error.h"
#include "Bolt.h"
#include "CoreThread.h"
#include "CreationRules.h"
#include "CurrentContextPusher.h"
#include "DeferredBase.h"
#include "fast_pointer_cast.h"
#include "result_or_default.h"
#include "JunctionBox.h"
#include "JunctionBoxManager.h"
#include "EventOutputStream.h"
#include "EventInputStream.h"
#include "ExceptionFilter.h"
#include "PolymorphicTypeForest.h"
#include "SimpleOwnershipValidator.h"
#include "TeardownNotifier.h"
#include "uuid.h"

#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <list>
#include <memory>
#include <map>
#include <string>
#include <functional>
#include TUPLE_HEADER
#include TYPE_INDEX_HEADER
#include FUNCTIONAL_HEADER
#include EXCEPTION_PTR_HEADER
#include SHARED_PTR_HEADER
#include STL_UNORDERED_MAP
#include STL_UNORDERED_SET

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
class EventOutputStreamBase;
class GlobalCoreContext;
class OutstandingCountTracker;

template<typename T>
class Autowired;

template<typename... Sigils>
struct Boltable;

#define CORE_CONTEXT_MAGIC 0xC04EC0DE

enum class ShutdownMode {
  // Shut down gracefully by allowing threads to run down dispatch queues
  Graceful,

  // Shut down immediately, do not attempt to run down thread dispatch queues
  Immediate
};

/// <summary>
/// A top-level container class representing an autowiring domain, a minimum broadcast domain, and a thread execution domain
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

public:
  virtual ~CoreContext(void);

  /// <summary>
  /// Factory to create a new context
  /// </summary>
  /// <param name="T">The context sigil.</param>
  template<class T>
  std::shared_ptr<CoreContext> Create(void) {
    return CreateInternal<T>(*new CoreContext(shared_from_this(), typeid(T)));
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
    return m_pParent->CreateInternal<T>(*new CoreContext(m_pParent, typeid(T), shared_from_this()));
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
  /// <summary>
  /// Register new context with parent and notify others of its creation.
  /// </summary>
  template<typename T>
  std::shared_ptr<CoreContext> CreateInternal(CoreContext& newContext) {
    t_childList::iterator childIterator;
    {
      // Lock the child list while we insert
      boost::lock_guard<boost::mutex> lk(m_childrenLock);
      
      // Reserve a place in the list for the child
      childIterator = m_children.insert(m_children.end(), std::weak_ptr<CoreContext>());
    }
    
    if (m_useOwnershipValidator)
      newContext.EnforceSimpleOwnership();
    
    // Create the shared pointer for the context--do not add the context to itself,
    // this creates a dangerous cyclic reference.
    std::shared_ptr<CoreContext> retVal(
      &newContext,
      [this, childIterator] (CoreContext* pContext) {
        {
          boost::lock_guard<boost::mutex> lk(m_childrenLock);
          this->m_children.erase(childIterator);
        }
        delete pContext;
      }
    );
    *childIterator = retVal;
    
    // Save anchored types in context
    retVal->AddAnchorInternal((T*)nullptr);
    
    // Fire all explicit bolts if not an "anonymous" context (has void sigil type)
    CurrentContextPusher pshr(retVal);
    BroadcastContextCreationNotice(typeid(T));
    
    // Fire event notifiying listeners that a new context was created
    GetGlobal()->Invoke(&AutowiringEvents::NewContext)(retVal);
    
    return retVal;
  }

  void AddAnchorInternal(const AutoAnchor<>*) {}

  template<typename... Ts>
  void AddAnchorInternal(const AutoAnchor<Ts...>*) {
    static_assert(sizeof...(Ts) != 0, "Cannot anchor nothing");

    bool dummy[] = {
      (m_anchors.insert(typeid(Ts)), false)...
    };
    (void)dummy;
  }

  void AddAnchorInternal(const void*) {}
  
  // These are the types which will be created in this context if an attempt is made to inject them
  // into any child context.
  std::set<std::type_index> m_anchors;

  // A pointer to the parent context
  const std::shared_ptr<CoreContext> m_pParent;

  // General purpose lock for this class
  mutable boost::mutex m_lock;

  // Condition, signalled when context state has been changed
  boost::condition m_stateChanged;

  // Set if threads in this context should be started when they are added
  bool m_shouldRunNewThreads;

  // Set if the context has been shut down
  bool m_isShutdown;

  // The context's internally held sigil type
  const std::type_info& m_sigil;
  
    // Flag, set if this context should use its ownership validator to guarantee that all autowired members
  // are correctly torn down.  This flag must be set at construction time.  Members added to the context
  // before this flag is assigned will NOT be checked.
  bool m_useOwnershipValidator;

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
  typedef std::unordered_set<JunctionBoxEntry<EventReceiver>> t_rcvrSet;
  t_rcvrSet m_eventReceivers;
  
  // Manages events for this context. One JunctionBoxManager is shared between peer contexts
  const std::shared_ptr<JunctionBoxManager> m_junctionBoxManager;
  
  // All known exception filters:
  std::unordered_set<ExceptionFilter*> m_filters;

  // Clever use of shared pointer to expose the number of outstanding CoreThread instances.
  // Destructor does nothing; this is by design.
  std::weak_ptr<Object> m_outstanding;

  // Actual core threads:
  typedef std::list<CoreThread*> t_threadList;
  t_threadList m_threads;

  // Child contexts:
  typedef std::list<std::weak_ptr<CoreContext>> t_childList;
  boost::mutex m_childrenLock;
  t_childList m_children;

  friend std::shared_ptr<GlobalCoreContext> GetGlobalContext(void);

  // The interior packet factory:
  const std::shared_ptr<AutoPacketFactory> m_packetFactory;

  // Lists of event receivers, by name:
  typedef std::unordered_map<std::type_index, std::list<BoltBase*>> t_contextNameListeners;
  t_contextNameListeners m_nameListeners;
  std::list<BoltBase*> m_allNameListeners;

  // Adds a bolt proper to this context
  template<typename T, typename... Sigils>
  void EnableInternal(T*, Bolt<Sigils...>*) {
    Inject<T>();
  }

  // Enables a boltable class
  template<typename T, typename... Sigils>
  void EnableInternal(T*, Boltable<Sigils...>*) {
    bool dummy[] = {
      false,
      (AutoRequireMicroBolt<T, Sigils>(), false)...
    };
    (void) dummy;
  }

  void EnableInternal(...) {}
  
  template<typename T, typename... Sigil>
  void AutoRequireMicroBolt(void);

  /// <summary>
  /// Unregisters all event receivers in this context
  /// </summary>
  void UnregisterEventReceivers(void);

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
  /// <param name="pOriginalParent">The original parent of the passed type</param>
  void AddEventReceiver(JunctionBoxEntry<EventReceiver> pRecvr);

  /// <summary>
  /// Adds the named event receiver to the collection of known receivers
  /// </summary>
  void AddEventReceiver(std::shared_ptr<EventReceiver> pRecvr) {
    return AddEventReceiver(JunctionBoxEntry<EventReceiver>(this, pRecvr));
  }

  /// <summary>
  /// Removes the named event receiver from the collection of known receivers
  /// </summary>
  void RemoveEventReceiver(JunctionBoxEntry<EventReceiver> pRecvr);

  /// <summary>
  /// Removes all recognized event receivers in the indicated range
  /// </summary>
  void RemoveEventReceivers(t_rcvrSet::const_iterator first, t_rcvrSet::const_iterator last);

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

  void RemovePacketSubscribers( const std::vector<AutoPacketSubscriber>& subscribers );

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
      AddInternal(ptr);
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

  template<class S>
  void DeferAutowiring(S& slot) {
    class Deferred:
      public DeferredBase {
    public:
      Deferred(CoreContext* pThis, S& slot) :
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

  // <summary>
  // Same as Inject, but doesn't checkout Typeforest
  // </summary>
  template<typename T>
  void AddInternal(const std::shared_ptr<T>& value){
    AddInternal(value, boost::unique_lock<boost::mutex>(m_lock));
  }
  
  template<typename T>
  void AddInternal(const std::shared_ptr<T>& value, boost::unique_lock<boost::mutex>&& lock) {
    // Extract ground for this value, we'll use it to select the correct forest for the value:
    typedef typename ground_type_of<T>::type groundType;

    // If Object appears in your ancestry then you MUST make object your ground type.  Typically
    // this is as simple as adding this line to the definition of T with public access:
    //
    //  typedef Object ground
    static_assert(
      !std::is_base_of<Object, T>::value ||
      std::is_same<typename ground_type_of<T>::type, Object>::value,
      "If T inherits from Object (for instance, via ContextMember or CoreThread), then T::grounds must be of type Object"
    );

    // Shared pointer to our entity, if it's a CoreThread
    std::shared_ptr<CoreThread> pCoreThread;

    {
      boost::unique_lock<boost::mutex> lk = std::move(lock);

      // Validate that this addition does not generate an ambiguity:
      std::shared_ptr<T> ptr;
      m_byType.Resolve(ptr);
      if(ptr == value)
        throw std::runtime_error("An attempt was made to add the same value to the same context more than once");
      if(ptr)
        throw std::runtime_error("An attempt was made to add the same type to the same context more than once");

      // Add a new member of the forest:
      m_byType.AddTree(value);

      // Context members:
      auto pContextMember = leap::fast_pointer_cast<ContextMember, T>(value);
      if(pContextMember) {
        AddContextMember(pContextMember);

        // CoreThreads:
        pCoreThread = leap::fast_pointer_cast<CoreThread, T>(value);
        if(pCoreThread)
          AddCoreThread(pCoreThread);
      }

      // Exception filters:
      auto pFilter = leap::fast_pointer_cast<ExceptionFilter, T>(value);
      if(pFilter)
        m_filters.insert(pFilter.get());

      // Bolts:
      auto pBase = leap::fast_pointer_cast<BoltBase, T>(value);
      if(pBase)
        AddBolt(pBase);
    }

    // Event receivers:
    auto pRecvr = leap::fast_pointer_cast<EventReceiver, T>(value);
    if(pRecvr)
      AddEventReceiver(pRecvr);

    // Subscribers:
    AddPacketSubscriber(AutoPacketSubscriberSelect<T>(value));

    // Notify any autowiring field that is currently waiting that we have a new member
    // to be considered.
    UpdateDeferredElements();

    // Ownership validation, as appropriate
    // We do not attempt to pend validation for CoreThread instances, because a CoreThread could potentially hold
    // the final outstanding reference to this context, and therefore may be responsible for this context's (and,
    // transitively, its own) destruction.
    if(m_useOwnershipValidator && !pCoreThread)
      SimpleOwnershipValidator::PendValidation(std::weak_ptr<T>(value));
  }

public:
  // Accessor methods:
  bool IsGlobalContext(void) const { return !m_pParent; }
  size_t GetMemberCount(void) const { return m_byType.size(); }
  const std::type_info& GetSigilType(void) const { return m_sigil; }
  
  /// <summary>
  /// Check if parent context's have AutoAnchored the type in their sigil.
  /// </summary>
  template<typename Sigil>
  std::shared_ptr<CoreContext> ResolveAnchor(void) {
    for(auto pCur = m_pParent; pCur; pCur = pCur->m_pParent) {
      if (pCur->m_anchors.find(typeid(Sigil)) != pCur->m_anchors.end()){
        return pCur;
      }
    }
    return shared_from_this();
  }
  
  /// <summary>
  /// Add an additional anchor type to the context
  /// </summary>
  template<typename... AnchorTypes>
  void AddAnchor(void) {
    bool dummy[] = {
      (m_anchors.insert(typeid(AnchorTypes)), false)...
    };
    (void) dummy;
  }

  /// <summary>
  /// Utility method which will inject the specified types into this context
  /// Arguments will be passed to the T constructor if provided
  /// </summary>
  template<typename T, typename... Args>
  std::shared_ptr<T> Construct(Args&&... args) {
    boost::unique_lock<boost::mutex> lk(m_lock);
    
    std::shared_ptr<T> ptr;
    m_byType.Resolve(ptr);
    if(ptr)
      return ptr;
    
    // We must make ourselves current for the duration of this call:
    CurrentContextPusher pshr(shared_from_this());

    // Cannot safely inject while holding the lock, so we have to unlock and then inject
    lk.unlock();
    ptr.reset(CreationRules::New<T>(std::forward<Args>(args)...));
    lk.lock();
    
    // Reattempt resolution, short-circuiting if an injection of this type took place:
    std::shared_ptr<T> ptr2;
    m_byType.Resolve(ptr2);
    if(ptr2)
      return ptr2;
    
    // Pass control to the insertion routine, which will handle injection from this point:
    AddInternal(ptr, std::move(lk));
    return ptr;
  }

  /// <summary>
  /// A simple utility method which will inject a single type when called
  /// </summary>
  /// <returns>
  /// The injected type
  /// </returns>
  template<typename T>
  std::shared_ptr<T> Inject(void) {
    return ResolveAnchor<T>() -> template Construct<T>();
  }

  /// <summary>
  /// A simple utility method which will inject the specified types into the current context when called
  /// </summary>
  template<typename T1, typename T2, typename... Ts>
  void Inject(void) {
    bool dummy[] = {
      (Inject<T1>(), false),
      (Inject<T2>(), false),
      (Inject<Ts>(), false)...
    };
    (void) dummy;
  }

  /// <summary>
  /// Static version of Inject that uses the current context
  /// </summary>
  template<typename... Ts>
  static void InjectCurrent(void) {
    auto ctxt = CurrentContext();
    bool dummy [] = {
      (ctxt->Inject<Ts>(), false)...
    };
    (void) dummy;
  }

  /// <summary>
  /// Adds an existing shared pointer to the context
  /// </summary>
  /// <remarks>
  /// This method unsafely ambiguates the construction strategy used for some member.  It's possible that
  /// someone calls AddExisting for a field which is AutoRequired in the current context, or makes a call
  /// to this method conditionally dependent on a type which may have been AutoRequired elsewhere.
  ///
  /// For reason of these ambiguities, and others, the method will be removed.
  /// </remarks>
  template<typename T>
  void DEPRECATED(AddExisting(std::shared_ptr<T> p_member), "Deprecated, use Inject or Construct instead");

  /// <summary>
  /// This method checks whether eventoutputstream listeners for the given type still exist.
  /// For a given type in a hash, returns a vector of weak ptrs.
  /// Goes through the weak ptrs, locks them, erases dead ones.
  /// If any live ones found return true. Otherwise false.
  /// NOTE: this func does lazy cleanup on weakptrs ptng to suff that has fallen out of scope.
  /// </summary>
  template <class T>
  bool CheckEventOutputStream(void){
    return m_junctionBoxManager->CheckEventOutputStream<T>();
  }

  /// <returns>
  /// True if the sigil type of this CoreContext matches the specified sigil type
  /// </returns>
  template<class Sigil>
  bool Is(void) const { return m_sigil == typeid(Sigil); }

  /// <summary>
  /// Enumerates all matching child contexts recursively and passes each child context to the specified lambda
  /// </summary>
  /// <param name="sigil">The sigil of the contexts to be passed to the specified lambda</param>
  /// <param name="fn">The lambda to receive a shared pointer to each matching child context</param>
  /// <returns>
  /// True if a complete enumeration has taken place, false if it was aborted by the passed lambda
  /// </returns>
  template<class Fn>
  bool EnumerateChildContexts(const std::type_info &sigil, Fn&& fn) {
    return EnumerateChildContexts(
      [&fn, &sigil] (std::shared_ptr<CoreContext> ctxt) -> bool {
        if(ctxt->GetSigilType() != sigil)
          // Sigil type doesn't match, skip this one
          return true;

        // The sigil type matches, then we'll filter it out to the original lambda
        return result_or_default(fn, true, ctxt);
      }
    );
  }

  /// <summary>
  /// Full enumeration of all child contexts, including anonymous contexts
  /// </summary>
  /// <remarks>
  /// Do not attempt to create any child contexts from the lambda function, this will cause
  /// deadlocks.  While it is technically possible to add objects to contexts from the lambda,
  /// there is a high probability that this will deadlock if any of the added objects directly
  /// or indirectly cause a child context to be created.
  ///
  /// CopyCoreThreadList is guaranteed to be a safe call to be made from this routine.
  /// </remarks>
  template<class Fn>
  bool EnumerateChildContexts(const Fn& fn) {
    boost::lock_guard<boost::mutex> lock(m_childrenLock);
    for(auto c = m_children.begin(); c != m_children.end(); c++) {
      // Recurse:
      auto shared = c->lock();
      if(shared && !shared->EnumerateChildContexts(fn))
        // Enumeration was abandoned
        return false;
    }

    // Call the lambda, default to true in case the lambda's return type is void:
    return result_or_default(fn, true, shared_from_this());
  }
  
  /// <summary>
  /// This is used for visualization purposes to get a list of all the contexts
  /// Fn must take a shared_ptr to a CoreContext as an argument.
  /// </summary>
  template<class Fn>
  void EnumerateContexts(Fn&& fn) {
    fn(shared_from_this());
    boost::lock_guard<boost::mutex> lock(m_childrenLock);
    for (auto c = m_children.begin(); c != m_children.end(); c++) {
      c->lock()->EnumerateContexts(fn);
    }
  }

  /// <returns>
  /// A copy of the list of child CoreThreads
  /// </returns>
  /// <remarks>
  /// No guarantee is made about how long the returned collection will be consistent with this
  /// context.  A thread may potentially be added to the context after the method returns.
  /// </remarks>
  std::vector<std::shared_ptr<CoreThread>> CopyCoreThreadList(void) const;

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
  bool IsShutdown(void) const {return m_isShutdown;}

  /// <returns>
  /// True if this context was ever started
  /// </returns>
  /// <remarks>
  /// A return value of "true" is guaranteed to be indefinitely correct.  A return value of
  /// "false" will only be correct for as long as it takes for someone to start this context.
  /// Unless externally synchronized, this operation may return false on a running context.
  /// </remarks>
  bool WasStarted(void) const {
    // We were started IF we will run new threads, OR we have been signalled to stop
    return m_shouldRunNewThreads || m_isShutdown;
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
    return ptr->GetContext().get() == this;
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
    return std::static_pointer_cast<JunctionBox<T>, JunctionBoxBase>(
      m_junctionBoxManager->Get<T>()
    );
  }

  /// <summary>
  /// Convenience method which allows an event to be fired without making the remote context current
  /// </summary>
  /// <remarks>
  /// The following two statements are equivalent:
  ///
  ///  CurrentContextPusher(ctxt),
  ///  (AutoFired<MyEventType>())(&MyEventType::MyEvent)();
  ///
  ///  ctxt->Invoke(&MyEventType::MyEvent)();
  ///
  /// </remarks>
  template<class MemFn>
  InvokeRelay<MemFn> Invoke(MemFn memFn) {
    return GetJunctionBox<typename Decompose<MemFn>::type>()->Invoke(memFn);
  }

  /// <summary>
  /// Utility routine, invoked typically by the service, which starts all registered
  /// core threads.
  /// </summary>
  void InitiateCoreThreads(void);

  /// <summary>
  /// This signals to the whole system that a shutdown operation is underway, and that shutdown procedures should
  /// begin immediately
  /// </summary>
  /// <param name="wait">Set if the function should wait for all child contexts to exit before returning</param>
  /// <remarks>
  /// This method will immediately prevent any new events from being recieved by this context or by any descendant
  /// context, whether those events are fired in this context or one above, and regardless of whether these events
  /// are fired or deferred.  Event receivers in this context will also not receive any messages.
  /// </remarks>
  void SignalShutdown(bool wait = false, ShutdownMode shutdownMode = ShutdownMode::Graceful);

  /// <summary>
  /// Alias for SignalShutdown(true, ShutdownMode::Immediate)
  /// </summary>
  void SignalTerminate(bool wait = true) { SignalShutdown(wait, ShutdownMode::Immediate); }

  /// <summary>
  /// Waits until all threads running in this context at the time of the call are sdtopped when the call returns
  /// </summary>
  /// <remarks>
  /// The only guarantees made by this method are that the threads which were running when the call was made will
  /// no longer be running upon return.  No guarantees are made about the state of other threads that might have
  /// been created after Wait was called; no guarantees are made about the run state or existence of any child
  /// contexts.  Child contexts may exist which contain running threads.
  /// </remarks>
  void Wait(void) {
    boost::unique_lock<boost::mutex> lk(m_lock);
    m_stateChanged.wait(lk, [this] () {return this->m_outstanding.expired();});
  }

  template<class Rep, class Period>
  bool Wait(const boost::chrono::duration<Rep, Period>& duration) {
    boost::unique_lock<boost::mutex> lk(m_lock);
    return m_stateChanged.wait_for(lk, duration, [this] {return this->m_outstanding.expired();});
  }

  /// <summary>
  /// This makes this core context current.
  /// </summary>
  /// <returns>The previously current context</returns>
  std::shared_ptr<CoreContext> SetCurrent(void);

  /// <summary>
  /// Makes no context current
  /// </summary>
  /// <remarks>
  /// Generally speaking, users wishing to release their reference to some context can do so simply
  /// by making the global context current.
  /// </remarks>
  static void EvictCurrent(void);

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
  ///
  /// Same as "AddEventReceiver" except doesn't added event to m_eventReceivers
  /// </remarks>
  template<class T>
  void Snoop(const std::shared_ptr<T>& pSnooper) {
    static_assert(std::is_base_of<EventReceiver, T>::value, "Cannot snoop on a type which is not an event receiver");

    // Snooping now
    auto rcvr = std::static_pointer_cast<EventReceiver, T>(pSnooper);

    JunctionBoxEntry<EventReceiver> receiver(this, rcvr);
    m_junctionBoxManager->AddEventReceiver(receiver);
    
    // Delegate ascending resolution, where possible.  This ensures that the parent context links
    // this event receiver to compatible senders in the parent context itself.
    if(m_pParent)
      m_pParent->Snoop(pSnooper);
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
    
    JunctionBoxEntry<EventReceiver> receiver(this, rcvr);
    m_junctionBoxManager->RemoveEventReceiver(receiver);
    
    // Delegate to the parent:
    if(m_pParent)
      m_pParent->Unsnoop(pSnooper);
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

  /// <summary>
  /// Registers a slot to be autowired
  /// </summary>
  template<class W>
  bool Autowire(W& slot) {
    if(AutowireNoDefer(slot))
      return true;
    
    // Failed, defer
    DeferAutowiring(slot);
    return false;
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

  /// <summary>
  /// Creates a new event stream based on the provided event type
  /// </summary>
  template<class T>
  std::shared_ptr<EventOutputStream<T>> CreateEventOutputStream(void) {
    return m_junctionBoxManager->CreateEventOutputStream<T>();
  }

  template<class T>
  std::shared_ptr<EventInputStream<T>> CreateEventInputStream(void) {
    return std::make_shared<EventInputStream<T>>();
  }
};

std::ostream& operator<<(std::ostream& os, const CoreContext& context);

#include "MicroBolt.h"

template<typename T, typename... Sigil>
void CoreContext::AutoRequireMicroBolt(void) {
  Inject<MicroBolt<T, Sigil...>>();
}

template<typename T>
void CoreContext::AddExisting(std::shared_ptr<T> p_member) {
  AddInternal(p_member);
}
