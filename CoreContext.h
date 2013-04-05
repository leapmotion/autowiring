#pragma once
#include "Autowirer.h"
#include "CoreThread.h"
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

namespace CoreContextHelpers {
  template<class T, bool isbase = cpp11::is_base_of<CoreThread, T>::value>
  struct Add;
}

class ContextMember;
class CoreThread;
class GlobalCoreContext;

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
  /// Creates a new context with the current context as the parent
  /// </summary>
  static cpp11::shared_ptr<CoreContext> NewContext() {
    return NewContext(CurrentContext());
  }

  /// <summary>
  /// Factory to create a new context
  /// </summary>
  /// <param name="pParent">An optional parent context.  If null, will default to the root context.</param>
  static cpp11::shared_ptr<CoreContext> NewContext(const cpp11::shared_ptr<CoreContext>& pParent);

private:
  // A back-reference to ourselves, weak in order to prevent a degenerate cyclic reference
  cpp11::weak_ptr<CoreContext> m_self;

  // General purpose lock for this class
  boost::mutex m_coreLock;

  // The current RUN count.  This is the number of times that InitiateCoreThreads has been called.
  // SignalShutdown must be called an equal number of times to actually shut down this context.
  int m_refCount;

  // This is a global STOP variable, used to signal shutdown when it's time to quit.
  bool m_shouldRun;
  bool m_shouldStop;

  // This is the stop condition, which is set when all core threads have actually exited
  boost::condition m_stop;

  // This is the stop condition, which is set when core threads have been told to stop
  boost::condition m_stopping;

  // Clever use of shared pointer to expose the number of outstanding CoreThread instances.
  // Destructor does nothing; this is by design.
  cpp11::shared_ptr<CoreContext> m_outstanding;

  // Actual core threads:
  typedef list<cpp11::shared_ptr<CoreThread> > t_threadList;
  t_threadList m_threads;

  friend cpp11::shared_ptr<GlobalCoreContext> GetGlobalContext(void);
  friend class GlobalCoreContext;

public:
  // Accessor methods:
  bool ShouldStop(void) const {return m_shouldStop;}

  template<class T>
  cpp11::shared_ptr<T> Add(T* pValue) {
    // Based on the above, we now decide where to send this add request.  The
    // reinterpret_cast
    return ((CoreContextHelpers::Add<T>&)*this)(pValue);
  }

  /// <summary>
  /// Adds an object of any kind to the IOC container
  /// </summary>
  /// <param name="pContextMember">The member which was added</param>
  /// <return>The shared pointer which contains the context member.</return>
  /// <remarks>
  /// It's safe to allow the returned shared_ptr to go out of scope; the core context
  /// will continue to hold a reference to it until Remove is invoked.
  /// </remarks>
  cpp11::shared_ptr<CoreThread> Add(CoreThread* pCoreThread);

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

    class Lambda {
    public:
      Lambda(CoreContext* pContext):
        pContext(pContext)
      {}

      CoreContext* pContext;

      bool operator()() {
        return pContext->m_outstanding.use_count() == 1;
      }
    };

    m_stop.wait(lk, Lambda(this));
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
  void SetCurrent(void) {
    SetCurrent(m_self.lock());
  }

  /// <summary>
  /// This makes a specific core context current
  /// </summary>
  static void SetCurrent(const cpp11::shared_ptr<CoreContext>& context) {
    ASSERT(context);
    cpp11::shared_ptr<CoreContext>* self = new cpp11::shared_ptr<CoreContext>(context);
    s_curContext.reset(self);
  };

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

/// <summary>
/// This is a special context for global use
/// </summary>
cpp11::shared_ptr<GlobalCoreContext> GetGlobalContext(void);

/// <summary>
/// Convenient access to the currently active context stored in the global context
/// </summary>
cpp11::shared_ptr<CoreContext> GetCurrentContext(void);

namespace CoreContextHelpers {
template<class T, bool isbase>
struct Add;

template<class T>
struct Add<T, true>:
  public CoreContext
{
  cpp11::shared_ptr<T> operator()(CoreThread* pValue) {
    return cpp11::static_pointer_cast<T, CoreThread>(CoreContext::Add(pValue));
  }
};

template<class T>
struct Add<T, false>:
  public CoreContext
{
  cpp11::shared_ptr<T> operator()(T* pValue) {
    return Autowirer::Add<T>(pValue);
  }
};

}