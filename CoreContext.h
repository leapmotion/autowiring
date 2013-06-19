// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _CORECONTEXT_H
#define _CORECONTEXT_H
#include "Autowirer.h"
#include "CoreThread.h"
#include "Bolt.h"
#include "CurrentContextPusher.h"
#include "DependentContext.h"
#include "TransientContextMember.h"
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>
#include <list>
#include <memory>
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

class BoltBase;
class ContextMember;
class CoreContext;
class CoreThread;
class GlobalCoreContext;
class OutstandingCountTracker;

/// <summary>
/// Convenient access to the currently active context stored in the global context
/// </summary>
std::shared_ptr<CoreContext> GetCurrentContext(void);

/// <summary>
/// This class is used to determine whether all core threads have exited
/// </summary>
class CoreContext:
  public Autowirer
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
        std::static_pointer_cast<CoreContext, Autowirer>(
          m_self.lock()
        )
      )
    );
    return retVal;
  }
  
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
  /// Adds the specified value without creating a new shared pointer for that value
  /// </summary>
  template<class T>
  void Add(const std::shared_ptr<T>& value) {
    Autowirer::Add(value);

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
    std::shared_ptr<CoreContext> newCurrent = 
      std::static_pointer_cast<CoreContext, Autowirer>(
        m_self.lock()
      );
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
};

std::ostream& operator<<(std::ostream& os, const CoreContext& context);

#endif
