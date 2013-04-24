#ifndef _CORECONTEXT_H
#define _CORECONTEXT_H
#include "Autowirer.h"
#include "CoreThread.h"
#include "CurrentContextPusher.h"
#include "DependentContext.h"
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>
#include <list>
#include <memory>
#include <string>
#include <vector>

#ifndef ASSERT
  #ifdef _DEBUG
    #include <assert.h>
    #define ASSERT(x) assert(x)
  #else
    #define ASSERT(x)
  #endif
#endif

using std::list;

class ContextMember;
class CoreContext;
class CoreThread;
class GlobalCoreContext;
class OutstandingCountTracker;

/// <summary>
/// Convenient access to the currently active context stored in the global context
/// </summary>
cpp11::shared_ptr<CoreContext> GetCurrentContext(void);

/// <summary>
/// This class is used to determine whether all core threads have exited
/// </summary>
class CoreContext:
  public Autowirer
{
  CoreContext(cpp11::shared_ptr<CoreContext> pParent);

  // A pointer to the current context, for construction purposes
  static boost::thread_specific_ptr<cpp11::shared_ptr<CoreContext> > s_curContext;

public:
  virtual ~CoreContext(void);

  /// <summary>
  /// Factory to create a new context
  /// </summary>
  /// <param name="pParent">An optional parent context.  If null, will default to the root context.</param>
  cpp11::shared_ptr<CoreContext> Create(void);

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
  DependentContext<T>* CreateDependentContext(void) {
    // Complex statement, here's what's going on:
    //  1) Create a new context, child of this one, and make it current
    //  2) Create a new dependent context
    //  3) T's ctor initializes all of T's AutoRequired members
    //  4) AutoRequired members insert new members into the new CoreContext
    //  5) DependentContext ctor is called, autowires in the current context into m_context
    //  6) pusher's dtor is called, resets the current context
    cpp11::shared_ptr<CoreContext> dependent = Create();
    return
      CurrentContextPusher(dependent.get()),
      new DependentContext<T>(dependent);
  }
  
  /// <summary>
  /// Virtually identical to CreateDependentContext, except adds the members of T to this context
  /// </summary>
  template<class T>
  DependentContext<T>* AugmentContext(void) {
    return
      CurrentContextPusher(this),
      new DependentContext<T>();
  }

private:
  // General purpose lock for this class
  boost::mutex m_coreLock;

  // The current RUN count.  This is the number of times that InitiateCoreThreads has been called.
  // SignalShutdown must be called an equal number of times to actually shut down this context.
  int m_refCount;

  // This is a global STOP variable, used to signal shutdown when it's time to quit.
  bool m_shouldStop;

  // This is the stop condition, which is set when all core threads have actually exited
  boost::condition m_stop;

  // This is the stop condition, which is set when core threads have been told to stop
  boost::condition m_stopping;

  // Clever use of shared pointer to expose the number of outstanding CoreThread instances.
  // Destructor does nothing; this is by design.
  boost::mutex m_outstandingLock;
  cpp11::weak_ptr<OutstandingCountTracker> m_outstanding;

  // Actual core threads:
  typedef list<CoreThread*> t_threadList;
  t_threadList m_threads;

  friend cpp11::shared_ptr<GlobalCoreContext> GetGlobalContext(void);
  friend class GlobalCoreContext;

  friend class OutstandingCountTracker;

public:
  // Accessor methods:
  bool ShouldStop(void) const {return m_shouldStop;}

  /// <summary>
  /// Increments the total number of contexts still outstanding
  /// </summary>
  /// <remarks>
  /// This is an indirect incrementation routine.  The count will be incremented for as
  /// long as the returned shared_ptr is not destroyed.  Once it's destroyed, the count
  /// is decremented.  The caller is encouraged not to copy the return value, as doing
  /// so can give spurious values for the current number of outstanding threads.
  /// </remarks>
  cpp11::shared_ptr<OutstandingCountTracker> IncrementOutstandingThreadCount(void);

  /// <summary>
  /// Adds the specified value without creating a new shared pointer for that value
  /// </summary>
  template<class T>
  void Add(cpp11::shared_ptr<T>& value) {
    Autowirer::Add(value);
    AddCoreThread(value.get());
  }

  /// <summary>
  /// Similar to the alternative overloaded Add method, except makes this context current before construction
  /// and creates a new shared_ptr for the constructed object
  /// </summary>
  template<class T>
  cpp11::shared_ptr<T> Add(void) {
    CurrentContextPusher pusher(this);
    cpp11::shared_ptr<T> retVal(new T);
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
  void AddCoreThread(void*) {}

  /// <summary>
  /// Utility routine, invoked typically by the service, which starts all registered
  /// core threads.
  /// </summary>
  /// <remarks>
  /// To stop core threads,
  /// </remarks>
  void InitiateCoreThreads(void);

  /// <summary>
  /// This signals to the whole system that a shutdown operation is underway, and that
  /// shutdown procedures should begin immediately
  /// </summary>
  void SignalShutdown(void);

  /// <summary>
  /// Waits for all threads holding references to exit
  /// </summary>
  void Wait(void) {
    boost::unique_lock<boost::mutex> lk(m_coreLock);
    while(!this->m_outstanding.expired())
      m_stop.wait(lk);
  }

  template<class Rep, class Period>
  bool Wait(const boost::chrono::duration<Rep, Period>& duration) {
    boost::unique_lock<boost::mutex> lk(m_coreLock);

    boost::cv_status stat;
    while(
      !this->m_outstanding.expired() &&
      stat != boost::cv_status::timeout
    )
      stat = m_stop.wait_for(lk, duration);
    return stat == boost::cv_status::no_timeout;
  }

  /// <summary>
  /// Makes the current context the parent of this context
  /// </summary>
  void Pop(void) {
    cpp11::static_pointer_cast<CoreContext, Autowirer>(m_pParent)->SetCurrent();
  }

  /// <summary>
  /// This makes this core context current.
  /// </summary>
  /// <returns>The previously current context</returns>
  cpp11::shared_ptr<CoreContext> SetCurrent(void) {
    cpp11::shared_ptr<CoreContext> newCurrent = 
      cpp11::static_pointer_cast<CoreContext, Autowirer>(
        m_self.lock()
      );
    ASSERT(newCurrent);
    return SetCurrent(newCurrent);
  }

  /// <summary>
  /// This makes a specific core context current
  /// </summary>
  /// <returns>The previously current context</returns>
  static cpp11::shared_ptr<CoreContext> SetCurrent(const cpp11::shared_ptr<CoreContext>& context) {
    ASSERT(context);
    cpp11::shared_ptr<CoreContext> retVal = GetCurrentContext();
    s_curContext.reset(new cpp11::shared_ptr<CoreContext>(context));
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
  static cpp11::shared_ptr<CoreContext> CurrentContext(void);
};
#endif
