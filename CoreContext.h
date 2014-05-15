#pragma once
#include "AnySharedPointer.h"
#include "AutoAnchor.h"
#include "AutoPacketSubscriber.h"
#include "AutowirableSlot.h"
#include "AutowiringEvents.h"
#include "autowiring_error.h"
#include "Bolt.h"
#include "CoreContextStateBlock.h"
#include "CoreRunnable.h"
#include "ContextMember.h"
#include "CreationRules.h"
#include "CurrentContextPusher.h"
#include "fast_pointer_cast.h"
#include "InvokeRelay.h"
#include "result_or_default.h"
#include "JunctionBoxManager.h"
#include "EventOutputStream.h"
#include "EventInputStream.h"
#include "ExceptionFilter.h"
#include "TeardownNotifier.h"
#include "TypeUnifier.h"

#include <list>
#include TYPE_INDEX_HEADER
#include MEMORY_HEADER
#include STL_UNORDERED_MAP
#include STL_UNORDERED_SET

struct CoreContextStateBlock;
class AutoPacketFactory;
class DeferrableAutowiring;
class BasicThread;
class BoltBase;
class CoreContext;
class EventOutputStreamBase;
class GlobalCoreContext;
class JunctionBoxBase;
class OutstandingCountTracker;

template<class T, class Fn>
class AutowirableSlotFn;

template<typename T>
class Autowired;

template<typename... Sigils>
struct Boltable;

template<class T>
class CoreContextT;

template<typename T>
class JunctionBox;

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
  public TeardownNotifier,
  public std::enable_shared_from_this<CoreContext>
{
protected:
  CoreContext(std::shared_ptr<CoreContext> pParent);
  CoreContext(std::shared_ptr<CoreContext> pParent, std::shared_ptr<CoreContext> pPeer);

public:
  virtual ~CoreContext(void);

  /// <summary>
  /// Convenience method to obtain a shared reference to the global context
  /// </summary>
  static std::shared_ptr<CoreContext> GetGlobal(void);

protected:
  // A pointer to the parent context
  const std::shared_ptr<CoreContext> m_pParent;

  // State block for this context:
  std::unique_ptr<CoreContextStateBlock> m_stateBlock;

  // Set if threads in this context should be started when they are added
  bool m_initiated;

  // Set if the context has been shut down
  bool m_isShutdown;

  // Child contexts:
  typedef std::list<std::weak_ptr<CoreContext>> t_childList;
  t_childList m_children;

  // Lists of event receivers, by name.  The type index of "void" is reserved for
  // bolts for all context types.
  typedef std::unordered_map<std::type_index, std::list<BoltBase*>> t_contextNameListeners;
  t_contextNameListeners m_nameListeners;

  // These are the types which will be created in this context if an attempt is made to inject them
  // into any child context.
  std::set<std::type_index> m_anchors;

  /// <summary>
  /// Represents a single entry, together with any deferred elements waiting on the satisfaction of this entry
  /// </summary>
  struct MemoEntry {
    MemoEntry(void) :
      pFirst(nullptr)
    {
    }

    // The first deferrable autowiring which requires this type, if one exists:
    DeferrableAutowiring* pFirst;

    // Once this memo entry is satisfied, this will contain the AnySharedPointer instance that performs
    // the satisfaction
    AnySharedPointer m_value;
  };

  // This is a list of concrete types, indexed by the true type of each element.
  std::vector<AnySharedPointer> m_concreteTypes;

  // This is a memoization map used to memoize any already-detected interfaces.  The map
  mutable std::unordered_map<std::type_index, MemoEntry> m_typeMemos;

  // All known context members, exception filters:
  std::vector<ContextMember*> m_contextMembers;
  std::vector<ExceptionFilter*> m_filters;

  // All known event receivers and receiver proxies originating from this context:
  typedef std::set<JunctionBoxEntry<EventReceiver>> t_rcvrSet;
  t_rcvrSet m_eventReceivers;
  
  // List of eventReceivers to be added when this context in initiated
  t_rcvrSet m_delayedEventReceivers;
  
  // Context members from other contexts that have snooped this context
  std::unordered_set<Object*> m_snoopers;

  // Manages events for this context. One JunctionBoxManager is shared between peer contexts
  const std::shared_ptr<JunctionBoxManager> m_junctionBoxManager;

  // Actual core threads:
  typedef std::list<CoreRunnable*> t_threadList;
  t_threadList m_threads;

  // Clever use of shared pointer to expose the number of outstanding CoreRunnable instances.
  // Destructor does nothing; this is by design.
  std::weak_ptr<Object> m_outstanding;

protected:
  /// <summary>
  /// Register new context with parent and notify others of its creation.
  /// </summary>
  std::shared_ptr<CoreContext> CreateInternal(CoreContext& newContext);

  template<typename T, typename... Ts>
  void AddAnchorInternal(const AutoAnchor<T, Ts...>*) { AddAnchor<T, Ts...>(); }

  void AddAnchorInternal(const void*) {}

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
  void UpdateDeferredElements(boost::unique_lock<boost::mutex>&& lk, const std::shared_ptr<Object>& entry);

  /// <summary>
  /// Adds the named event receiver to the collection of known receivers
  /// </summary>
  /// <param name="pRecvr">The junction box entry corresponding to the receiver type</param>
  void AddEventReceiver(JunctionBoxEntry<EventReceiver> pRecvr);

  /// <summary>
  /// Add delayed event receivers
  /// </summary>
  template<class iter>
  void AddEventReceivers(iter first, iter last);

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
  void AddCoreRunnable(const std::shared_ptr<CoreRunnable>& pCoreRunnable);

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
  /// Forwarding routine, recursively adds a packet subscriber to the internal packet factory
  /// </summary>
  void AddPacketSubscriber(const AutoPacketSubscriber& rhs);
  
  /// <summary>
  /// Forwarding routine, only removes from this context
  /// </summary>
  void UnsnoopAutoPacket(Object* oSnooper, const std::type_info& ti);
  
  /// <summary>
  /// Counterparts to the AddPacketSubscriber routine
  /// </summary>
  void RemovePacketSubscribers(const std::vector<AutoPacketSubscriber>& subscribers);

  /// <summary>
  /// Increments the total number of contexts still outstanding
  /// </summary>
  /// <remarks>
  /// This is an indirect incrementation routine.  The count will be incremented for as
  /// long as the returned shared_ptr is not destroyed.  Once it's destroyed, the count
  /// is decremented.  The caller is encouraged not to copy the return value, as doing
  /// so can give inflated values for the current number of outstanding threads.
  ///
  /// The caller is responsible for exterior synchronization
  /// </remarks>
  std::shared_ptr<Object> IncrementOutstandingThreadCount(void);
  
  /// <summary>
  /// Mapping and extraction structure used to provide a runtime version of an Object-implementing shared pointer
  /// </summary>
  struct AddInternalTraits {
    template<class T>
    AddInternalTraits(const AutoPacketSubscriber& subscriber, const std::shared_ptr<T>& value) :
      type(typeid(T)),
      subscriber(subscriber),
      value(value),
      pObject(leap::fast_pointer_cast<Object>(value)),
      pContextMember(leap::fast_pointer_cast<ContextMember>(value)),
      pCoreRunnable(leap::fast_pointer_cast<CoreRunnable>(value)),
      pFilter(leap::fast_pointer_cast<ExceptionFilter>(value)),
      pBoltBase(leap::fast_pointer_cast<BoltBase>(value)),
      pRecvr(leap::fast_pointer_cast<EventReceiver>(value))
    {
      if(!pObject)
        throw autowiring_error("Cannot add a type which does not implement Object");
    }

    // The declared original type:
    const std::type_info& type;

    // A holder to store the original shared pointer, to ensure that type information propagates
    // correctly on the right-hand side of our map
    const AnySharedPointer value;

    // The packet subscriber introduction method, if appropriate:
    const AutoPacketSubscriber subscriber;

    // There are a lot of interfaces we support, here they all are:
    const std::shared_ptr<Object> pObject;
    const std::shared_ptr<ContextMember> pContextMember;
    const std::shared_ptr<CoreRunnable> pCoreRunnable;
    const std::shared_ptr<ExceptionFilter> pFilter;
    const std::shared_ptr<BoltBase> pBoltBase;
    const std::shared_ptr<EventReceiver> pRecvr;
  };

  /// <summary>
  /// Internal type introduction routine
  /// </summary>
  void AddInternal(const AddInternalTraits& traits);

  /// <summary>
  /// Scans the memo collection for the specified entry, or adds a deferred resolution marker if resolution was not possible
  /// </summary>
  /// <returns>
  /// The memo entry where this type was found
  /// </returns>
  /// <param name="reference">An initialized shared pointer slot which may be used in type detection</param>
  void FindByType(AnySharedPointer& reference) const;

  /// <summary>
  /// Unsynchronized version of FindByType
  /// </summary>
  void FindByTypeUnsafe(AnySharedPointer& reference) const;

  /// <summary>
  /// Returns or constructs a new AutoPacketFactory instance
  /// </summary>
  std::shared_ptr<AutoPacketFactory> GetPacketFactory(void);

  /// <summary>
  /// Adds the specified deferrable autowiring as a general recipient of autowiring events
  /// </summary>
  void AddDeferred(const AnySharedPointer& reference, DeferrableAutowiring* deferrable);

  /// <summary>
  /// Adds a snooper to the snoopers set
  /// </summary>
  void InsertSnooper(std::shared_ptr<Object> snooper);

  /// <summary>
  /// Removes a snooper to the snoopers set
  /// </summary>
  void RemoveSnooper(std::shared_ptr<Object> snooper);

public:
  // Accessor methods:
  bool IsGlobalContext(void) const { return !m_pParent; }
  size_t GetMemberCount(void) const { return m_concreteTypes.size(); }
  virtual const std::type_info& GetSigilType(void) const = 0;

  /// <summary>
  /// Factory to create a new context
  /// </summary>
  /// <param name="T">The context sigil.</param>
  template<class T>
  std::shared_ptr<CoreContext> Create(void) {
    return CreateInternal(*new CoreContextT<T>(shared_from_this()));
  }

  /// <summary>
  /// Factory to create a peer context
  /// </summary>
  /// <remarks>
  /// A peer context allows clients to create autowiring contexts which are in the same event
  /// domain with respect to each other, but are not in the same autowiring domain.  This can
  /// be useful where multiple instances of a particular object are desired, but inserting
  /// such objects into a simple child context is cumbersome because the objects at parent
  /// scope are listening to events originating from objects at child scope. Events can be fired,
  /// but not received, from an unintiated context if its peer is initiated.
  /// </remarks>
  template<class T>
  std::shared_ptr<CoreContext> CreatePeer(void) {
    return m_pParent->CreateInternal(*new CoreContextT<T>(m_pParent, shared_from_this()));
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
      (AddAnchor(typeid(AnchorTypes)), false)...
    };
    (void) dummy;
  }

  /// <summary>
  /// Adds the specified anchor type to the context
  void AddAnchor(const std::type_info& ti);

  /// <summary>
  /// Utility method which will inject the specified types into this context
  /// Arguments will be passed to the T constructor if provided
  /// </summary>
  template<typename T, typename... Args>
  std::shared_ptr<T> Construct(Args&&... args) {
    // If T doesn't inherit Object, then we need to compose a unifying type which does
    typedef typename SelectTypeUnifier<T>::type TActual;
    static_assert(std::is_base_of<Object, TActual>::value, "Constructive type does not implement Object as expected");
    static_assert(
      std::is_base_of<Object, T>::value || !has_static_new<T>::value,
      "If type T provides a static new method, then the constructed type MUST directly inherit Object"
    );

    // First see if the object has already been injected:
    std::shared_ptr<TActual> retVal;
    FindByType(retVal);
    if(retVal)
      return retVal;

    // We must make ourselves current for the remainder of this call:
    CurrentContextPusher pshr(shared_from_this());

    // Cannot safely inject while holding the lock, so we have to unlock and then inject
    retVal.reset(CreationRules::New<TActual>(std::forward<Args>(args)...));

    try {
      // Pass control to the insertion routine, which will handle injection from this point:
      AddInternal(AddInternalTraits(AutoPacketSubscriberSelect<T>(retVal), retVal));
    }
    catch(autowiring_error&) {
      // We know why this exception occurred.  It's because, while we were constructing our
      // type, someone else was constructing the same type at the same time.  As a consequence,
      // we will simply eat this exception, and handle it silently by returning the type that
      // someone else has already attempted to construct, as per the documented behavior of
      // Construct.
      FindByType(retVal);
    }
    return retVal;
  }

  /// <summary>
  /// A simple utility method which will inject a single type when called
  /// </summary>
  /// <returns>
  /// The injected type
  /// </returns>
  template<typename T>
  std::shared_ptr<T> Inject(void) {
    return ResolveAnchor<T>()->template Construct<T>();
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
  bool Is(void) const { return GetSigilType() == typeid(Sigil); }

  /// <returns>
  /// A list of descendant contexts whose sigil type matches the specified sigil type
  /// </returns>
  template<class Sigil>
  std::vector<std::shared_ptr<CoreContext>> EnumerateChildContexts(void) {
    std::vector<std::shared_ptr<CoreContext>> retVal;
    EnumerateChildContexts([&retVal](std::shared_ptr<CoreContext> ctxt) {
      retVal.push_back(ctxt);
    });
    return retVal;
  }

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
  /// CopyBasicThreadList is guaranteed to be a safe call to be made from this routine.
  /// </remarks>
  template<class Fn>
  bool EnumerateChildContexts(const Fn& fn) {
    boost::lock_guard<boost::mutex> lock(m_stateBlock->m_lock);
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
    boost::lock_guard<boost::mutex> lock(m_stateBlock->m_lock);
    for (auto c = m_children.begin(); c != m_children.end(); c++) {
      c->lock()->EnumerateContexts(fn);
    }
  }

  /// <summary>
  /// Sends AutowiringEvents to build current state
  /// </summary>
  void BuildCurrentState(void);

  /// <returns>
  /// A copy of the list of child CoreRunnables
  /// </returns>
  /// <remarks>
  /// No guarantee is made about how long the returned collection will be consistent with this
  /// context.  A thread may potentially be added to the context after the method returns.
  /// </remarks>
  std::vector<std::shared_ptr<BasicThread>> CopyBasicThreadList(void) const;

  /// <returns>
  /// True if CoreRunnable instances in this context should begin teardown operations
  /// </returns>
  bool IsShutdown(void) const {return m_isShutdown;}
  
  bool IsInitiated(void) const {return m_initiated;}

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
    return m_initiated || m_isShutdown;
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
  template<typename MemFn>
  InvokeRelay<MemFn> Invoke(MemFn memFn){
    typedef typename Decompose<MemFn>::type EventType;
    
    if (!std::is_same<AutowiringEvents,EventType>::value)
      GetGlobal()->Invoke(&AutowiringEvents::EventFired)(*this, typeid(EventType));
    
    return MakeInvokeRelay(GetJunctionBox<EventType>(), memFn);
  }

  /// <summary>
  /// Utility routine, invoked typically by the service, which starts all registered
  /// core threads.
  /// </summary>
  void Initiate(void);
  void DEPRECATED(InitiateCoreThreads(void), "InitiateCoreThreads is deprecated, use Initiate instead");

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
  /// Waits until the context is transitioned to the Stopped state and all threads and child threads have terminated.
  /// </summary>
  void Wait(void);

  /// <summary>
  /// Timed overload
  /// </summary>
  bool Wait(const boost::chrono::nanoseconds duration);
  
  /// <summary>
  /// Wait until the context is initiated or is shutting down
  /// </summary>
  /// <returns>True if initiated, false if shutting down</returns>
  bool DelayUntilInitiated(void);

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
  /// context is assigned before invoking a CoreRunnable instance's Run method, and it's also assigned
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
  /// </remarks>
  template<class T>
  void Snoop(const std::shared_ptr<T>& pSnooper) {
    static_assert(std::is_base_of<EventReceiver, T>::value ||
                  has_autofilter<T>::value,
                  "Cannot snoop on a type which is not an EventReceiver or implements AutoFilter");
    
    InsertSnooper(pSnooper);

    // Add EventReceiver
    if (std::is_base_of<EventReceiver, T>::value) {
      JunctionBoxEntry<EventReceiver> receiver(this, pSnooper);
      AddEventReceiver(receiver);
    }
    
    // Add PacketSubscriber;
    if (has_autofilter<T>::value) {
      AddPacketSubscriber(AutoPacketSubscriberSelect<T>(pSnooper));
    }
  }

  /// <summary>
  /// Unregisters an event receiver previously registered to receive snooped events
  /// </summary>
  /// <remarks>
  /// It is an error to call this method without a prior call to Snoop
  /// </remarks>
  template<class T>
  void Unsnoop(const std::shared_ptr<T>& pSnooper) {
    static_assert(std::is_base_of<EventReceiver, T>::value ||
                  has_autofilter<T>::value,
                  "Cannot snoop on a type which is not an EventReceiver or implements AutoFilter");
    
    RemoveSnooper(pSnooper);

    Object* oSnooper = std::static_pointer_cast<Object>(pSnooper).get();
    
    // Cleanup if its an EventReceiver
    if (std::is_base_of<EventReceiver, T>::value)
      UnsnoopEvents(oSnooper, JunctionBoxEntry<EventReceiver>(this, pSnooper));
    
    // Cleanup if its a packet listener
    if (has_autofilter<T>::value)
      UnsnoopAutoPacket(oSnooper, typeid(T));
  }
  
  /// <summary>
  /// Remove EventReceiver from parents unless its a member of the parent
  /// </summary>
  void UnsnoopEvents(Object* oSnooper, const JunctionBoxEntry<EventReceiver>& receiver);

  /// <summary>
  /// Locates an available context member in this context
  /// </summary>
  template<class T>
  void FindByType(std::shared_ptr<T>& slot) const {
    AnySharedPointerT<T> ptr;
    FindByType(ptr);
    slot = ptr->as<T>();
  }

  /// <summary>
  /// Identical to Autowire, but will not register the passed slot for deferred resolution
  /// </summary>
  template<class T>
  bool FindByTypeRecursive(std::shared_ptr<T>& slot) {
    // First-chance resolution in this context and ancestor contexts:
    for(CoreContext* pCur = this; pCur; pCur = pCur->m_pParent.get()) {
      pCur->FindByType(slot);
      if(slot)
        return true;
    }

    return false;
  }

  /// <summary>
  /// Registers a slot to be autowired
  /// </summary>
  template<class T>
  bool Autowire(AutowirableSlot<T>& slot) {
    if(FindByTypeRecursive(slot))
      return true;

    // Failed, defer
    AddDeferred(AnySharedPointerT<T>(), &slot);
    return false;
  }

  /// <summary>
  /// Adds a post-attachment listener in this context for a particular autowired member
  /// </summary>
  /// <returns>
  /// A pointer to a deferrable autowiring function which the caller may safely ignore if it's not needed
  /// </returns>
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
  ///
  /// The returned value may be used later in CancelAutowiringNotification in order to explicitly clean
  /// up memory.
  /// </remarks>
  template<class T, class Fn>
  const AutowirableSlotFn<T, Fn>* NotifyWhenAutowired(Fn&& listener) {
    auto retVal = MakeAutowirableSlotFn<T>(
      shared_from_this(),
      std::forward<Fn>(listener)
    );

    AddDeferred(AnySharedPointerT<T>(), retVal);
    return retVal;
  }

  /// <summary>
  /// Unregisters a slot as a recipient of potential autowiring
  /// </summary>
  void CancelAutowiringNotification(DeferrableAutowiring* pDeferrable);

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

/// <summary>
/// Constant type optimization for named sigil types
/// </summary>
template<class T>
class CoreContextT:
  public CoreContext
{
public:
  CoreContextT(std::shared_ptr<CoreContext> pParent) :
    CoreContext(pParent)
  {
    // Save anchored types in context
    AddAnchorInternal((T*)nullptr);
  }

  CoreContextT(std::shared_ptr<CoreContext> pParent, std::shared_ptr<CoreContext> pPeer) :
    CoreContext(pParent, pPeer)
  {
    // Save anchored types in context
    AddAnchorInternal((T*)nullptr);
  }

  const std::type_info& GetSigilType(void) const override { return typeid(T); }
};

std::ostream& operator<<(std::ostream& os, const CoreContext& context);

#include "MicroBolt.h"

template<typename T, typename... Sigil>
void CoreContext::AutoRequireMicroBolt(void) {
  Inject<MicroBolt<T, Sigil...>>();
}
