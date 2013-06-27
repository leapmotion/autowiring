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
#include "safe_dynamic_cast.h"
#include "SharedPtrWrap.h"
#include "TransientContextMember.h"
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>
#include <list>
#include <memory>
#include <map>
#include <string>
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

using std::list;

class AutowirableSlot;
class BoltBase;
class ContextMember;
class CoreContext;
class CoreThread;
class GlobalCoreContext;
class OutstandingCountTracker;

namespace CoreContextHelpers {
  template<class T, bool isPolymorphic = std::is_base_of<Object, T>::value>
  struct FindByCastInternal;

  template<class T, bool isPolymorphic = std::is_polymorphic<T>::value>
  struct AddPolymorphic;
}

template<class T>
class Autowired;


/// <summary>
/// Convenient access to the currently active context stored in the global context
/// </summary>
std::shared_ptr<CoreContext> GetCurrentContext(void);

/// <summary>
/// This class is used to determine whether all core threads have exited
/// </summary>
class CoreContext:
  public Object
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
  // TODO:  Use type_index instead.
  // It's more efficient to use type_index, here, instead of trying to index directly by
  // the class name.
  typedef std::multimap<std::string, SharedPtrWrapBase*> t_mpType;
  t_mpType m_byType;

  // All ContextMember objects known in this autowirer:
  std::unordered_set<ContextMember*> m_contextMembers;

  // Collection of objects waiting to be autowired, and a specific lock exclusively for this collection
  boost::mutex m_deferredLock;
  typedef std::map<const AutowirableSlot*, DeferredBase*> t_deferred;
  t_deferred m_deferred;

  // All known event senders and receivers
  typedef std::unordered_set<std::shared_ptr<EventReceiver>, SharedPtrHash<EventReceiver>> t_rcvrSet;
  t_rcvrSet m_eventReceivers;
  std::set<EventSenderBase*> m_eventSenders;

  // All known exception filters:
  std::unordered_set<ExceptionFilter*> m_filters;

  /// <summary>
  /// Invokes all deferred autowiring fields, generally called after a new member has been added
  /// </summary>
  void UpdateDeferredElements(void);

  /// Adds an object of any kind to the IOC container
  /// </summary>
  /// <param name="value">The member to be added</param>
  /// <remarks>
  /// It's safe to allow the returned shared_ptr to go out of scope; the core context
  /// will continue to hold a reference to it until it is destroyed
  /// </remarks>
  template<class T>
  SharedPtrWrap<T>* AddInternal(std::shared_ptr<T> value) {
    // Add to the map:
    SharedPtrWrap<T>* pWrap = new SharedPtrWrap<T>(m_self, value);
    (boost::lock_guard<boost::mutex>)m_lock,
    m_byType.insert(
      t_mpType::value_type(
        std::string(typeid(*value.get()).name()),
        pWrap
      )
    );

    // Polymorphic insertion, as required:
    ((CoreContextHelpers::AddPolymorphic<T>&)*this)(value);

    // Notify any autowiring field that is currently waiting that we have a new member
    // to be considered.
    UpdateDeferredElements();
    return pWrap;
  }

  /// <summary>
  /// Adds the passed event manager to the collection of known event senders, and links it up
  /// </summary>
  void AddToEventSenders(EventSenderBase* pSender);

  /// <summary>
  /// Removes all recognized event receivers in the indicated range
  /// </summary>
  void RemoveEventSenders(t_rcvrSet::iterator first, t_rcvrSet::iterator last);

  template<class W>
  bool DoAutowire(W& slot) {
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

  friend class SharedPtrWrapBase;

private:
  // General purpose lock for this class
  boost::mutex m_coreLock;

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
  typedef list<CoreThread*> t_threadList;
  t_threadList m_threads;

  // Child contexts:
  typedef list<std::weak_ptr<CoreContext> > t_childList;
  boost::mutex m_childrenLock;
  t_childList m_children;

  friend std::shared_ptr<GlobalCoreContext> GetGlobalContext(void);
  friend class GlobalCoreContext;

  friend class OutstandingCountTracker;

public:
  // Accessor methods:
  bool ShouldStop(void) const {return m_shouldStop;}

  /// <summary>
  /// Broadcasts a notice to any listener in the current context regarding a creation event on a particular context name
  /// </summary>
  void BroadcastContextCreationNotice(const char* contextName, const std::shared_ptr<CoreContext>& context) const;

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
  /// <param name="pContextMember">The member which was added</param>
  /// <remarks>
  /// It's safe to allow the returned shared_ptr to go out of scope; the core context
  /// will continue to hold a reference to it until Remove is invoked.
  /// </remarks>
  template<class T>
  void Add(const std::shared_ptr<T>& value) {
    auto pWrap = AddInternal(value);
    AddContextMember(pWrap);

    // Is the passed value a CoreThread?
    CoreThread* pCoreThread = safe_dynamic_cast<CoreThread, T>::Cast(value.get());
    if(pCoreThread)
      AddCoreThread(pCoreThread);

    // Is the passed value a Bolt?
    BoltBase* pBase = safe_dynamic_cast<BoltBase, T>::Cast(value.get());
    if(pBase)
      AddBolt(pBase);
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
  void AddCoreThread(CoreThread* pCoreThread, bool allowNotReady = false);

  /// <summary>
  /// Adds the specified context creation listener to receive creation events broadcast from this context
  /// </summary>
  /// <param name="pBase">The instance being added</param>
  void AddBolt(BoltBase* pBase);

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
  void SignalTerminate(void);

  /// <summary>
  /// Waits for all threads holding references to exit
  /// </summary>
  void Wait(void) {
    boost::unique_lock<boost::mutex> lk(m_coreLock);
    m_stateChanged.wait(lk, [this] () {return this->m_outstanding.expired();});
  }

  template<class Rep, class Period>
  bool Wait(const boost::chrono::duration<Rep, Period>& duration) {
    boost::unique_lock<boost::mutex> lk(m_coreLock);
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
  void Dump(std::ostream& os) const;// Accessor methods:

  /// <summary>
  /// Obtains a pointer to the parent context
  /// </summary>
  std::shared_ptr<CoreContext>& GetParentContext(void) {return m_pParent;}

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
  /// <param name="pSender">The sender of the event</param>
  /// <param name="pRecipient">The recipient of the event</param>
  void FilterFiringException(const EventSenderBase* pSender, EventReceiver* pRecipient);

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
  void Snoop(const std::shared_ptr<EventReceiver>& pSnooper);

  /// <summary>
  /// Unregisters an event receiver previously registered to receive snooped events
  /// </summary>
  /// <remarks>
  /// It is an error to call this method without a prior call to Snoop
  /// </remarks>
  void Unsnoop(const std::shared_ptr<EventReceiver>& pSnooper);

  /// <summary>
  /// Overload of Add based on ContextMember
  /// </summary>
  void AddContextMember(SharedPtrWrapBase* pWrap);

  /// <summary>
  /// Attempts to find a member in the container that can be passed to the specified type
  /// </summary>
  /// <remarks>
  /// This method will throw an exception if there is more than one object castable to
  /// type T.  This method cannot be invoked with any type that does not inherit from Object
  /// due to limitations on the way that dynamic_cast works internally.
  /// </remarks>
  template<class T>
  std::shared_ptr<T> FindByCast(void) {
    return ((CoreContextHelpers::FindByCastInternal<T>&)*this)();
  }

  /// <summary>
  /// Locates an available context member by its exact type, if known
  /// </summary>
  template<class T>
  std::shared_ptr<T> FindByType(const Autowired<T>&) {
    return FindByType<T>();
  }

  template<class T>
  std::shared_ptr<T> FindByType(void) {
    // Attempt a resolution by type first:
    std::string typeName = typeid(T).name();
    t_mpType::iterator q;
    {
      boost::lock_guard<boost::mutex> lk(m_lock);
      q = m_byType.find(typeName);
    }

    if(q == m_byType.end())
      // We couldn't find by a perfect name match, now we'll try to find something in the
      // table which can be safely casted to the type we're looking for.  A common case
      // where this is needed is where the autowiring calls for an interface, and we have
      // a concrete implementation of this interface.
      // Note that this only works for polymorphic types!
      // POCO types do not have a VFT and cannot be used in a dynamic_cast.
      return FindByCast<T>();

    // If find has been requested by type, there should be only one match.
    std::shared_ptr<T>& retVal = *(SharedPtrWrap<T>*)q->second;
    q++;
    if(q != m_byType.end() && q->first == typeName)
      // Ambiguous match, exception:
      throw_rethrowable std::runtime_error("An autowiring operation resulted in an ambiguous match");
    return retVal;
  }

  /// <summary>
  /// Registers a pointer to be autowired
  /// </summary>
  /// <remarks>
  /// The autowired member must generally inherit from Autowired, or there
  /// may be issues.
  /// </remarks>
  template<class W>
  bool Autowire(W& slot) {
    if(DoAutowire(slot))
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
          this->pThis->DoAutowire(this->slot);
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

namespace CoreContextHelpers {


template<class T, bool isPolymorphic>
struct AddPolymorphic:
  public CoreContext
{
public:
  void AddEventReceiver(std::shared_ptr<EventReceiver> pRecvr) {
    // Add to our local collection:
    m_eventReceivers.insert(pRecvr);

    // Scan the list of compatible senders:
    for(auto q = m_eventSenders.begin(); q != m_eventSenders.end(); q++)
      **q += pRecvr;

    // Delegate ascending resolution, where possible.  This ensures that the parent context links
    // this event receiver to compatible senders in the parent context itself.
    if(m_pParent)
      ((AddPolymorphic<T, true>&)*m_pParent).AddEventReceiver(pRecvr);
  }

  void RemoveEventReceiver(std::shared_ptr<EventReceiver> pRecvr) {
    // Remove from the local collection
    m_eventReceivers.erase(pRecvr);

    // Notify all compatible senders that we're going away:
    for(auto q = m_eventSenders.begin(); q != m_eventSenders.end(); q++)
      **q -= pRecvr;

    // Delegate to the parent:
    if(m_pParent)
      ((AddPolymorphic<T, true>&)*m_pParent).RemoveEventReceiver(pRecvr);
  };

  inline void operator()(std::shared_ptr<T> value) {
    // Event senders first:
    {
      EventSenderBase* pEventSenderBase = dynamic_cast<EventSenderBase*>(value.get());
      if(pEventSenderBase)
        CoreContext::AddToEventSenders(pEventSenderBase);
    }

    // Event receivers second:
    std::shared_ptr<EventReceiver> pRecvr = std::dynamic_pointer_cast<EventReceiver, T>(value);
    if(pRecvr)
      AddEventReceiver(pRecvr);

    // Finally, any exception filters:
    ExceptionFilter* pFilter = dynamic_cast<ExceptionFilter*>(value.get());
    if(pFilter)
      m_filters.insert(pFilter);
  }
};

template<class T>
struct AddPolymorphic<T, false>:
  public CoreContext
{
public:
  inline void operator()(const std::shared_ptr<T>&) {}
};

template<class T, bool isPolymorphic>
struct FindByCastInternal:
  public CoreContext
{
  std::shared_ptr<T> operator()(void) {
    boost::lock_guard<boost::mutex> lk(m_lock);
    std::shared_ptr<Object> matchedObject;

    static_assert((!std::is_same<Object, T>::value), "FindByCastInternal on type Object is an overly broad search criteria");

    std::shared_ptr<Object> obj;
    for(t_mpType::iterator q = m_byType.begin(); q != m_byType.end(); ++q) {
      SharedPtrWrapBase* pBase = q->second;

      // See if this wrap contains an object, which we could use to access other types
      obj = pBase->AsObject();
      if(!obj)
        continue;

      // Okay, we can work with this, it's an Object
      try {
        // Try the cast
        if(!dynamic_cast<T*>(obj.get()))
          // Sometimes the above will throw an exception, sometimes it will return null.
          // We handle both cases by wrapping this in a try-except block.
          continue;

        // Verify that we didn't try storing something already
        if(matchedObject)
          throw_rethrowable std::runtime_error("An autowiring operation resulted in an ambiguous match");

        // Record this value to be returned
        matchedObject.swap(obj);
      } catch(std::bad_cast&) {
      } catch(...) {
        // Very bad exception happened
      };
    }

    return
      matchedObject ?
      std::dynamic_pointer_cast<T, Object>(matchedObject) :
      std::shared_ptr<T>();
  }
};

template<class T>
struct FindByCastInternal<T, false>:
  public CoreContext
{
  std::shared_ptr<T> operator()(void) {
    return std::shared_ptr<T>();
  }
};

}

#endif
