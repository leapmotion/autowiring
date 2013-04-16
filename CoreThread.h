#ifndef _CORETHREAD_H
#define _CORETHREAD_H
#include "ContextMember.h"
#include <vector>
#include <memory>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

using cpp11::shared_ptr;

class CoreContext;
class CoreThread;

/// <summary>
/// This is an abstract class that has a single Run method for implementation by a
/// derived class.  The object will remain in the context as long as the thread is
/// running, even if there are no other references to it.
/// </summary>
class CoreThread:
  public ContextMember
{
public:
  /// <param name="pName">An optional name for this thread</param>
  CoreThread(const char* pName = nullptr):
    ContextMember(pName),
    m_stop(true),
    m_running(false)
  {
  }

  virtual ~CoreThread(void) {}

private:
  boost::mutex m_stopLock;
  boost::condition_variable m_stopCondition;
  bool m_stop;
  bool m_running;

  friend class ThreadStatusMaintainer;

public:
  // Accessor methods:
  bool ShouldStop(void) const;
  bool IsReady(void) const {return !m_stop;}
  bool IsRunning(void) const {return m_running;}

  /// <summary>
  /// A convenience method that will sleep this thread for the specified duration
  /// </summary>
  void ThreadSleep(long millisecond);

  /// <summary>
  /// Delay execution of the corresponding core thread until it's ready
  /// </summary>
  bool DelayUntilReady(void) {
    boost::unique_lock<boost::mutex> lk(m_stopLock);
    m_stopCondition.wait(lk, [this] () {return IsReady();});
    return !m_context.expired();
  }

  /// <summary>
  /// Causes a new thread to be created in which the Run method will be invoked
  /// </summary>
  /// <returns>True to indicate that the thread was started successfully</returns>
  /// <remarks>
  /// Note that this operation has an inherit race condition.
  bool Start(void);

  /// <summary>
  /// Begins the core thread
  /// </summary>
  /// <param name="context">A shared pointer to the context containing this thread</param>
  virtual void Run() = 0;

  /// <summary>
  /// Generally speaking, this method doesn't need to be called as it's invoked
  /// by the core context on behalf of the constructed object.  However, if this
  /// class is not constructed in the null context, then Ready must be manually
  /// invoked.
  /// </summary>
  void Ready(void) {
    m_stop = false;

    boost::lock_guard<boost::mutex> lk(m_stopLock);
    m_stopCondition.notify_all();
  }

  /// <summary>
  /// This is an override method that will cause ShouldStop to return true,
  /// regardless of what the global stop setting is.
  /// </summary>
  virtual void Stop(void) {
    m_stop = true;
    boost::lock_guard<boost::mutex> lk(m_stopLock);
    m_stopCondition.notify_all();
  }
};

#endif