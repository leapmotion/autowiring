// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _CORETHREAD_H
#define _CORETHREAD_H
#include "ContextMember.h"
#include "DispatchQueue.h"
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <memory>

using std::shared_ptr;

class CoreContext;
class CoreThread;

/// <summary>
/// This is an abstract class that has a single Run method for implementation by a
/// derived class.  The object will remain in the context as long as the thread is
/// running, even if there are no other references to it.
/// </summary>
class CoreThread:
  public ContextMember,
  public DispatchQueue
{
public:
  /// <param name="pName">An optional name for this thread</param>
  CoreThread(const char* pName = nullptr);

  virtual ~CoreThread(void) {}

protected:
  // General purpose thread lock and update condition for the lock
  boost::mutex m_lock;
  boost::condition_variable m_stateCondition;

  // Flag indicating that we need to stop right now
  bool m_stop;

  // Ready condition
  bool m_ready;

  // Run condition:
  bool m_running;

  // Completion condition, true when this thread is no longer running and has run at least once
  bool m_completed;

  // Acceptor flag:
  bool m_canAccept;

  friend class ThreadStatusMaintainer;

private:
  /// <summary>
  /// Assigns the name of the thread, for use in debugger windows
  /// </summary>
  /// <remarks>
  /// Some platforms allow the assignment of a thread name for viewing by a debugger
  /// window.  This method is a platform-independent way of doing this for the current
  /// thread.
  /// </remarks>
  void SetCurrentThreadName(void) const;

  /// <summary>
  /// Private routine that sets up the necessary extranea before a call to Run
  /// </summary>
  void DoRun(void);

protected:
  /// <summary>
  /// Indicates that the system should accept the delivery of deferred procedure calls
  /// </summary>
  void AcceptDispatchDelivery(void) {
    m_canAccept = true;
    boost::lock_guard<boost::mutex> lk(m_lock);
    m_stateCondition.notify_all();
  }

  /// <summary>
  /// Indicates that no more deferred procedure calls should be pended
  /// </summary>
  /// <remarks>
  /// This method guarantees that, at some point after it returns, the caller will no
  /// longer receive pended events.  No guarantees are made about how long after this
  /// call is made that the last pended dispatch event will be delivered.
  ///
  /// Callers interested in this guarantee should invoke this method and then wait for
  /// the dispatch queue to become empty.
  /// </remarks>
  void RejectDispatchDelivery(void) {
    m_canAccept = false;
    boost::lock_guard<boost::mutex> lk(m_lock);
    m_stateCondition.notify_all();
  }

public:
  // Accessor methods:
  bool ShouldStop(void) const;
  bool IsReady(void) const {return m_ready;}
  bool IsRunning(void) const {return m_running;}

  // Override from EventDispatcher
  bool CanAccept(void) const override {return m_canAccept;}

  /// <summary>
  /// A convenience method that will sleep this thread for the specified duration
  /// </summary>
  void ThreadSleep(long millisecond);

  /// <summary>
  /// Delay execution of the corresponding core thread until it's ready
  /// </summary>
  bool DelayUntilReady(void) {
    boost::unique_lock<boost::mutex> lk(m_lock);
    m_stateCondition.wait(lk, [this] () {
      return !ShouldStop() || m_ready;
    });
    return !ShouldStop() && !m_context.expired();
  }
  
  // Base overrides:
  bool DelayUntilCanAccept(void) override;

  /// <summary>
  /// Causes a new thread to be created in which the Run method will be invoked
  /// </summary>
  /// <returns>True to indicate that the thread was started successfully</returns>
  /// <remarks>
  /// Note that this operation has an inherit race condition.  Be careful to ensure that
  /// Start will not be called from more than one place on the same object.  The thread
  /// will be invoked from the context which was active at the time the thread was created.
  /// </remarks>
  bool Start(void);

  /// <summary>
  /// Begins the core thread
  /// </summary>
  /// <param name="context">A shared pointer to the context containing this thread</param>
  /// <remarks>
  /// The default implementation of Run will simply call WaitForEvent in a loop until it's
  /// told to quit.
  /// </remarks>
  virtual void Run();

  /// <summary>
  /// Generally speaking, this method doesn't need to be called as it's invoked
  /// by the core context on behalf of the constructed object.  However, if this
  /// class is not constructed in the null context, then Ready must be manually
  /// invoked.
  /// </summary>
  void Ready(void) {
    boost::lock_guard<boost::mutex> lk(m_lock);
    m_ready = true;
    m_stateCondition.notify_all();
  }

  /// <summary>
  /// Waits until the core thread is launched and then terminates
  /// </summary>
  /// <remarks>
  /// Unlike Join, this method may be invoked even if the CoreThread isn't running
  /// </remarks>
  void Wait(void) {
    boost::unique_lock<boost::mutex> lk(m_lock);
    m_stateCondition.wait(
      lk,
      [this] () {return this->m_completed;}
    );
  }

  /// <summary>
  /// This is an override method that will cause ShouldStop to return true,
  /// regardless of what the global stop setting is.
  /// </summary>
  virtual void Stop(void) {
    m_stop = true;
    boost::lock_guard<boost::mutex> lk(m_lock);
    m_stateCondition.notify_all();

    // Abort the dispatch queue so anyone waiting will wake up
    DispatchQueue::Abort();
  }
};

#endif
