// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _CORECONTEXT_H
#define _CORECONTEXT_H
#include "CoreThread.h"
#include "Bolt.h"
#include "CurrentContextPusher.h"
#include "DeferredBase.h"
#include "DependentContext.h"
#include "ExceptionFilter.h"
#include "EventSender.h"
#include "PolymorphicTypeForest.h"
#include "SharedPtrWrap.h"
#include "TeardownNotifier.h"
#include "TransientContextMember.h"
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

template<class Fn>
struct AtExit {
  AtExit(Fn&& fn):
    fn(fn)
  {}

  ~AtExit(void) {
    fn();
  }

  Fn fn;
};

template<class Fn>
AtExit<Fn> MakeAtExit(Fn&& fn) {
  return AtExit<Fn>(std::forward<Fn>(fn));
}

/// <summary>
/// Convenient access to the currently active context stored in the global context
/// </summary>
std::shared_ptr<CoreContext> GetCurrentContext(void);

/// <summary>
/// This class is used to determine whether all core threads have exited
/// </summary>
class CoreContext:
  public Object,
  public TeardownNotifier
{
  CoreContext(std::shared_ptr<CoreContext> pParent);

  // A pointer to the current context, for construction purposes
  static boost::thread_specific_ptr<std::shared_ptr<CoreContext> > s_curContext;

public:
  virtual ~CoreContext(void);

  /// <summary>
  /// Factory to create a new context
  /// </summary>
  /// <param name="pParent">An optional parent context.  If null, will default to the root context.</param>
  std::shared_ptr<CoreContext> Create(void);

  /// <summary>
  /// Creates a dependent context using the specified global structure as a reference entity
  /// </summary>
  /// <returns>A new dependent context with the requested members</returns>
  /// <remarks>
  /// The structure passed as type T is usually a simple structure containing nothing except
  /// AutoRequired members which will populate the context with some required basic members.
  /// The structure generally looks like this:
  ///
  /// struct UseCaseName {
  ///   AutoRequired<GeneratorClass> m_generator;
  ///   AutoRequired<FinderClass> m_finder;
  /// };
  ///
  /// The dependent context has an autowired CoreContext member in it; this member is needed
  /// in order to provide the sole guaranteed strong reference to the context that prevents
  /// the whole thing from being freed.
  ///
  /// The current context is altered during this method call, but will be reset before it
  /// returns.  The InitiateCoreThreads routine is not invoked on the created Context; the
  /// caller is responsible for starting core threads if this is desired.
  /// </remarks>
  template<class T>
  std::shared_ptr<DependentContext<T> > CreateDependentContext(void) {
    // Complex statement, here's what's going on:
    //  1) Create a new context, child of this one, and make it current
    //  2) Create a new dependent context
    //  3) T's ctor initializes all of T's AutoRequired members
    //  4) AutoRequired members insert new members into the new CoreContext
    //  5) DependentContext ctor is called, autowires in the current context into m_context
    //  6) pusher's dtor is called, resets the current context
    std::shared_ptr<CoreContext> dependent = Create();
    CurrentContextPusher pusher(dependent);
    std::shared_ptr<DependentContext<T> > retVal(
      new DependentContext<T>(dependent)
    );
    return retVal;
  }

  /// <summary>
  /// Virtually identical to CreateDependentContext, except adds the members of T to this context
  /// </summary>
  template<class T>
  std::shared_ptr<DependentContext<T> > AugmentContext(void) {
    CurrentContextPusher pusher(this);
    std::shared_ptr<DependentContext<T> > retVal(
      new DependentContext<T>(
        std::static_pointer_cast<CoreContext, CoreContext>(
          m_self.lock()
        )
      )
    );
    return retVal;
  }

protected:
  // General purpose lock for this class
  mutable boost::mutex m_lock;

  // A pointer to the parent context
  std::shared_ptr<CoreContext> m_pParent;

  // A back-reference to ourselves, weak in order to prevent a degenerate cyclic reference
  std::weak_ptr<CoreContext> m_self;

  // This is a map of the context members by type and, where appropriate, by name
  // This map keeps all of its objects resident at least until the context goes
  // away.
  PolymorphicTypeForest<Object> m_byType;

  // All ContextMember objects known in this autowirer:
  std::unordered_set<ContextMember*> m_contextMembers;

  // Collection of objects waiting to be autowired, and a specific lock exclusively for this collection
  boost::mutex m_deferredLock;
  typedef std::map<const AutowirableSlot*, DeferredBase*> t_deferred;
  t_deferred m_deferred;

  // All known event receivers and receiver proxies:
  typedef std::unordered_set<std::shared_ptr<EventReceiver>, SharedPtrHash<EventReceiver>> t_rcvrSet;
  t_rcvrSet m_eventReceivers;

  typedef std::unordered_map<std::type_index, std::shared_ptr<EventReceiverProxyBase>> t_proxies;
  t_proxies m_proxies;

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

  // Lists of event receivers, by name:
  typedef std::unordered_map<std::string, std::list<BoltBase*>> t_contextNameListeners;
  t_contextNameListeners m_nameListeners;

  // Clever use of shared pointer to expose the number of outstanding CoreThread instances.
  // Destructor does nothing; this is by design.
  boost::mutex m_outstandingLock;
  std::weak_ptr<OutstandingCountTracker> m_outstanding;

  // Actual core threads:
  typedef std::list<CoreThread*> t_threadList;
  t_threadList m_threads;

  // Child contexts:
  typedef std::list<std::weak_ptr<CoreContext> > t_childList;
  boost::mutex m_childrenLock;
  t_childList m_children;

  friend std::shared_ptr<GlobalCoreContext> GetGlobalContext(void);
  friend class GlobalCoreContext;
  friend class OutstandingCountTracker;

  /// <summary>
  /// Invokes all deferred autowiring fields, generally called after a new member has been added
  /// </summary>
  void UpdateDeferredElements(void);
  
  void AddEventReceiver(std::shared_ptr<EventReceiver> pRecvr) {
    {
      // Add to our local collection:
      boost::lock_guard<boost::mutex> lk(m_lock);
      m_eventReceivers.insert(pRecvr);

      // Scan the list of compatible senders:
      for(auto q = m_proxies.begin(); q != m_proxies.end(); q++)
        *q->second += pRecvr;
    }

    // Delegate ascending resolution, where possible.  This ensures that the parent context links
    // this event receiver to compatible senders in the parent context itself.
    if(m_pParent)
      m_pParent->AddEventReceiver(pRecvr);
  }

  void RemoveEventReceiver(std::shared_ptr<EventReceiver> pRecvr) {
    // Remove from the local collection
    (boost::lock_guard<boost::mutex>)m_lock,
    m_eventReceivers.erase(pRecvr);

    // Notify all compatible senders that we're going away:
    for(auto q = m_proxies.begin(); q != m_proxies.end(); q++)
      *q->second -= pRecvr;

    // Delegate to the parent:
    if(m_pParent)
      m_pParent->RemoveEventReceiver(pRecvr);
  };

  /// <summary>
  /// Removes all recognized event receivers in the indicated range
  /// </summary>
  void RemoveEventReceivers(t_rcvrSet::iterator first, t_rcvrSet::iterator last);

  friend class SharedPtrWrapBase;

public:
  // Accessor methods:
  size_t GetMemberCount(void) const {return m_byType.size();}
  bool IsRunning(void) const {return !!m_refCount;}

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
  /// Broadcasts a notice to any listener in the current context regarding a creation event on a particular context name
  /// </summary>
  void BroadcastContextCreationNotice(const char* contextName, const std::shared_ptr<CoreContext>& context) const;

  /// <summary>
  /// Obtains a shared pointer to an event sender _in this context_ matching the specified type
  /// </summary>
  template<class T>
  std::shared_ptr<EventReceiverProxy<T>> GetEventRecieverProxy(void) {
    std::shared_ptr<EventReceiverProxy<T>> retVal;
    boost::lock_guard<boost::mutex> lk(m_lock);
    auto q = m_proxies.find(typeid(T));
    if(q != m_proxies.end())
      // No dynamic cast is needed here, we already have independent knowledge of the
      // destination type because it's the key type of our map
      return std::static_pointer_cast<EventReceiverProxy<T>, EventReceiverProxyBase>(q->second);

    // Construct new type:
    retVal.reset(new EventReceiverProxy<T>);
    m_proxies[typeid(T)] = retVal;
      
    // Attach compatible receivers:
    for(auto q = m_eventReceivers.begin(); q != m_eventReceivers.end(); q++)
      *retVal += *q;

    // Construction complete
    return retVal;
  }

  /// <summary>
  /// Increments the total number of contexts still outstanding
  /// </summary>
  /// <remarks>
  /// This is an indirect incrementation routine.  The count will be incremented for as
  /// long as the returned shared_ptr is not destroyed.  Once it's destroyed, the count
  /// is decremented.  The caller is encouraged not to copy the return value, as doing
  /// so can give spurious values for the current number of outstanding threads.
  /// </remarks>
  std::shared_ptr<OutstandingCountTracker> IncrementOutstandingThreadCount(void);

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
    {
      boost::lock_guard<boost::mutex> lk(m_lock);

      // Add a new member of the forest:
      auto obj = std::fast_pointer_cast<Object, T>(value);
      if(obj) {
        m_byType.AddTree(obj);

        // Context members:
        auto pContextMember = std::fast_pointer_cast<ContextMember, T>(value);
        if(pContextMember) {
          AddContextMember(pContextMember);

          // CoreThreads:
          auto pCoreThread = std::fast_pointer_cast<CoreThread, T>(value);
          if(pCoreThread)
            AddCoreThread(pCoreThread);
        }
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

    // Notify any autowiring field that is currently waiting that we have a new member
    // to be considered.
    UpdateDeferredElements();
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
  /// Adds an object of any kind to the IOC container
  /// </summary>
  /// <param name="pContextMember">The member which was added</param>
  /// <param name="notReady">Allows the insertion of a thread, even if that thread isn't ready yet</param>
  /// <return>The shared pointer which contains the context member.</return>
  /// <remarks>
  /// It's safe to allow the returned shared_ptr to go out of scope; the core context
  /// will continue to hold a reference to it until Remove is invoked.
  /// </remarks>
  void AddCoreThread(const std::shared_ptr<CoreThread>& pCoreThread, bool allowNotReady = false);

  /// <summary>
  /// Adds the specified context creation listener to receive creation events broadcast from this context
  /// </summary>
  /// <param name="pBase">The instance being added</param>
  void AddBolt(const std::shared_ptr<BoltBase>& pBase);

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
    std::shared_ptr<CoreContext> newCurrent = m_self.lock();
    ASSERT(newCurrent);
    return SetCurrent(newCurrent);
  }

  /// <summary>
  /// This makes a specific core context current
  /// </summary>
  /// <returns>The previously current context</returns>
  static std::shared_ptr<CoreContext> SetCurrent(const std::shared_ptr<CoreContext>& context) {
    ASSERT(context);
    std::shared_ptr<CoreContext> retVal = GetCurrentContext();
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
  /// Utility debug method for writing a snapshot of this context to the specified output stream
  /// </summary>
  void Dump(std::ostream& os) const;

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
  void FilterFiringException(const EventReceiverProxyBase* pProxy, EventReceiver* pRecipient);

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
  /// Overload of Add based on ContextMember
  /// </summary>
  void AddContextMember(const std::shared_ptr<ContextMember>& ptr);

  /// <summary>
  /// Locates an available context member by its exact type, if known
  /// </summary>
  template<class T>
  std::shared_ptr<T> FindByType(const Autowired<T>&) {
    return FindByType<T>();
  }

  template<class T>
  std::shared_ptr<T> FindByType(void) {
    std::shared_ptr<T> retVal;
    boost::lock_guard<boost::mutex> lk(m_lock);
    if(!m_byType.Resolve(retVal))
      throw_rethrowable std::runtime_error("An autowiring operation resulted in an ambiguous match");
    return retVal;
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

  /// <summary>
  /// Identical to Autowire, but will not register the passed slot for deferred resolution
  /// </summary>
  template<class W>
  bool AutowireNoDefer(W& slot) {
    typename W::t_ptrType retVal;
    for(CoreContext* pCur = this; pCur; pCur = pCur->m_pParent.get()) {
      retVal = pCur->FindByType(slot);
      if(retVal) {
        slot.swap(retVal);
        return true;
      }
    }
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
        throw_rethrowable std::runtime_error("A slot is being autowired, but a deferred instance already exists at this location");
    }
    pDeferred = new Deferred(this, slot);
  }

  /// <summary>
  /// Adds a post-attachment listener in this context for a particular autowired member
  /// </summary>
  /// <remarks>
  /// This method will succeed if slot was constructed in this context or any parent context.  If this
  /// assumption is false, an exception will be thrown.
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

  static void DebugPrintCurrentExceptionInformation();
};

std::ostream& operator<<(std::ostream& os, const CoreContext& context);

#endif
