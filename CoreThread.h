// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _CORETHREAD_H
#define _CORETHREAD_H
#include "ContextMember.h"
#include "DispatchQueue.h"
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <memory>

using std::shared_ptr;

class CoreContext;
class CoreThread;

enum class ThreadPriority {
  // This is the default thread priority, it's treated as a value lower than any of the
  // other priority so that an initial request to elevate can alter the OS-supplied default
  // without causing the types of contention problems resulting from a high-priority operation
  // transitioning to a low-priority state.
  Default,

  Idle,
  Lowest,
  BelowNormal,
  Normal,
  AboveNormal,
  Highest,
  TimeCritical,

  // This is a special case for multimedia applications.  Some operating systems, like Windows,
  // can provide additional scheduling guarantees to applications which declare themselves as
  // multimedia-intensive in nature.  For other systems, Multimedia is identical to TimeCritical.
  Multimedia
};

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

private:
  struct State {
    // General purpose thread lock and update condition for the lock
    boost::mutex m_lock;
    boost::condition_variable m_stateCondition;
  };

  // Internally held thread status block.  This has to be a shared pointer because we need to signal
  // the held state condition after releasing all shared pointers to ourselves, and this could mean
  // we're actually signalling this event after we free ourselves.
  const std::shared_ptr<State> m_state;

protected:
  // Convenience references:
  boost::mutex& m_lock;
  boost::condition_variable& m_stateCondition;

  // Flag indicating that we need to stop right now
  bool m_stop = false;

  // Run condition:
  bool m_running = false;

  // Completion condition, true when this thread is no longer running and has run at least once
  bool m_completed = false;

  // Acceptor flag:
  bool m_canAccept = false;

  // The current thread priority
  ThreadPriority m_priority;

  // The current thread, if running
  boost::thread m_thisThread;

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

  /// <summary>
  /// Sets the thread priority of this thread
  /// </summary>
  /// <remarks>
  /// This method must only be called from the running thread's context, and then, only inside
  /// the ElevatePriority constructor
  /// </remarks>
  void SetThreadPriority(ThreadPriority threadPriority);

protected:
  void DEPRECATED(Ready(void) const, "Do not call this method, the concept of thread readiness is now deprecated") {}

  /// <summary>
  /// Indicates that the system should accept the delivery of deferred procedure calls
  /// </summary>
  void AcceptDispatchDelivery(void) {
    m_canAccept = true;
    m_state->m_stateCondition.notify_all();
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
  ///
  /// This method is idempotent
  /// </remarks>
  void RejectDispatchDelivery(void) {
    m_canAccept = false;
    m_state->m_stateCondition.notify_all();
  }

  /// <summary>
  /// RAII-style class for use with threads that need temporary priority boosts
  /// </summary>
  /// <remarks>
  /// The thread priority is changed when the requested priority is higher than the
  /// current priority.  The destructor automatically restores the previous priority.
  /// This class cannot be moved or copied, in order to guarantee proper RAII.
  /// </remarks>
  class ElevatePriority {
  public:
    ElevatePriority(ElevatePriority&) = delete;

    ElevatePriority(CoreThread& thread, ThreadPriority priority) :
      m_thread(thread),
      m_oldPriority(thread.m_priority)
    {
      // Elevate if the new level is higher than the old level:
      if(priority > m_oldPriority)
        m_thread.SetThreadPriority(priority);
    }

    ~ElevatePriority(void) {
      // Delevate if the old level is lower than the current level:
      if(m_thread.m_priority > m_oldPriority)
        m_thread.SetThreadPriority(m_oldPriority);
    }

  private:
    ThreadPriority m_oldPriority;
    CoreThread& m_thread;
  };

public:
  // Accessor methods:
  bool ShouldStop(void) const;
  bool IsRunning(void) const { return m_running; }

  // Override from EventDispatcher
  bool CanAccept(void) const override {return m_canAccept;}

  /// <summary>
  /// A convenience method that will sleep this thread for the specified duration
  /// </summary>
  void ThreadSleep(long millisecond);
  
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
  bool Start(std::shared_ptr<Object> outstanding);

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
  /// Waits until the core thread is launched and then terminates
  /// </summary>
  /// <remarks>
  /// Unlike Join, this method may be invoked even if the CoreThread isn't running
  /// </remarks>
  void Wait(void) {
    boost::unique_lock<boost::mutex> lk(m_lock);
    m_state->m_stateCondition.wait(
      lk,
      [this]() {return this->m_completed; }
    );
  }

  /// <summary>
  /// Timed version of Wait
  /// </summary>
  /// <returns>False if the timeout elapsed, true otherwise</returns>
  template<class DurationType>
  bool WaitFor(DurationType duration) {
    boost::unique_lock<boost::mutex> lk(m_lock);
    return m_state->m_stateCondition.wait_for(
      lk,
      duration,
      [this] () {return this->m_completed;}
    );
  }
  
  /// <summary>
  /// Timed version of Wait
  /// </summary>
  /// <returns>False if the timeout elapsed, true otherwise</returns>
  template<class TimeType>
  bool WaitUntil(TimeType timepoint) {
    boost::unique_lock<boost::mutex> lk(m_lock);
    return m_stateCondition.wait_until(
      lk,
      timepoint,
      [this]() {return this->m_completed; }
    );
  }

  /// <summary>
  /// Event which may be used to perform custom handling when the thread is told to stop
  /// </summary>
  /// <param name="graceful">Set to true to rundown the dispatch queue before quitting</param>
  /// <remarks>
  /// This method is called when the thread should stop.  When invoked, the value of
  /// CoreThread::ShouldStop is guaranteed to be true.
  ///
  /// Callers are not required to call CoreThread::OnStop.  This method is guaranteed to do
  /// nothing by default.
  /// </remarks>
  virtual void OnStop(void) {}

  /// <summary>
  /// This is an override method that will cause ShouldStop to return true,
  /// regardless of what the global stop setting is.
  /// </summary>
  virtual void Stop(bool graceful = false) {
    // Trivial return check:
    if(m_stop)
      return;

    // Perform initial stop:
    m_stop = true;
    OnStop();

    if(graceful) {
      // Signal the dispatch queue to run down
      RejectDispatchDelivery();

      // Pend a call which will invoke Abort once the dispatch queue is done:
      DispatchQueue::Pend([this] {
        this->Abort();
        
        // Notify callers of our new state:
        boost::lock_guard<boost::mutex> lk(this->m_lock);
        this->m_state->m_stateCondition.notify_all();
      });
    } else {
      // Abort the dispatch queue so anyone waiting will wake up
      DispatchQueue::Abort();
      
      // Notify all callers of our status update, only needed if we don't call
      // RejectDispatchDelivery first
      boost::lock_guard<boost::mutex> lk(m_lock);
      m_state->m_stateCondition.notify_all();
    }
  }
};

#endif
