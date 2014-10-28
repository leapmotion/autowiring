// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "ContextMember.h"
#include "CoreRunnable.h"
#include MEMORY_HEADER
#include MUTEX_HEADER
#include CHRONO_HEADER

struct BasicThreadStateBlock;
class BasicThread;
class CoreContext;

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
class BasicThread:
  public ContextMember,
  public virtual CoreRunnable
{
public:
  /// <param name="pName">An optional name for this thread</param>
  BasicThread(const char* pName = nullptr);
  virtual ~BasicThread(void);

protected:
  // Internally held thread status block.  This has to be a shared pointer because we need to signal
  // the held state condition after releasing all shared pointers to ourselves, and this could mean
  // we're actually signalling this event after we free ourselves.
  const std::shared_ptr<BasicThreadStateBlock> m_state;

  // Flag indicating that we need to stop right now
  bool m_stop;

  // Run condition:
  bool m_running;

  // Completion condition, true when this thread is no longer running and has run at least once
  bool m_completed;

  // The current thread priority
  ThreadPriority m_priority;

  /// <summary>
  /// Assigns the name of the thread, for use in debugger windows
  /// </summary>
  /// <remarks>
  /// Some platforms allow the assignment of a thread name for viewing by a debugger
  /// window.  This method is a platform-independent way of doing this for the current
  /// thread.
  /// </remarks>
  void SetCurrentThreadName(void) const;

private:
  /// <summary>
  /// Sets the thread priority of this thread
  /// </summary>
  /// <remarks>
  /// This method must only be called from the running thread's context, and then, only inside
  /// the ElevatePriority constructor
  /// </remarks>
  void SetThreadPriority(ThreadPriority threadPriority);

protected:
  /// <summary>
  /// Recovers a general lock used to synchronize entities in this thread internally
  /// </summary>
  std::mutex& GetLock(void);

  /// <summary>
  /// Routine that sets up the necessary extranea before a call to Run
  /// </summary>
  /// <remarks>
  /// Clients who wish to trigger teardown may release the reference to the passed refTracker
  /// instance.  If this thread is the last thread holding a reference to this context, then
  /// after the point where the reference is released, [this] will be deleted.  Clients should
  /// be careful to hold their own references to refTracker before calling DoRun if they
  /// intend to reference member data on function return.  This method will always release the
  /// shared pointer passed to it, typically as a last step before return, potentially
  /// triggering the case described above.
  /// </remarks>
  virtual void DoRun(std::shared_ptr<Object>&& refTracker);

  /// <summary>
  /// Performs all cleanup operations that must take place after DoRun
  /// </summary>
  /// <param name="pusher">The last reference to the enclosing context held by this thread</param>
  virtual void DoRunLoopCleanup(std::shared_ptr<CoreContext>&& ctxt, std::shared_ptr<Object>&& refTracker);

  void DEPRECATED(Ready(void) const, "Do not call this method, the concept of thread readiness is now deprecated") {}

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

    ElevatePriority(BasicThread& thread, ThreadPriority priority) :
      m_oldPriority(thread.m_priority),
      m_thread(thread)
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
    BasicThread& m_thread;
  };

  /// <summary>
  /// Performs a state condition wait using the specified lambda to judge completeness
  /// </summary>
  void WaitForStateUpdate(const std::function<bool()>& fn);

  /// <summary>
  /// Obtains a mutex, invokes the specified lambda, and then updates the basic thread's state condition
  /// </summary>
  void PerformStatusUpdate(const std::function<void()>& fn);

  /// <summary>
  /// A convenience method that will sleep this thread for the specified duration
  /// </summary>
  /// <returns>False if the thread was terminated before the timeout elapsed</returns>
  /// <remarks>
  /// Events are dispatched by this method while the sleep is taking place, which makes this
  /// method similar to an alertable wait on Windows.  Callers are cautioned against holding
  /// locks while calling this method; if this is done, a deadlock could result.
  ///
  /// Callers should not invoke this method outside of this thread's thread context, or an
  /// interruption exception could result.
  /// </remarks>
  bool ThreadSleep(std::chrono::nanoseconds timeout);

public:
  // Accessor methods:
  bool ShouldStop(void) const override;
  bool IsRunning(void) const override;

  /// <summary>
  /// Causes a new thread to be created in which the Run method will be invoked
  /// </summary>
  /// <returns>True to indicate that the thread was started successfully, false if it was already started or cancelled</returns>
  /// <remarks>
  /// Note that this operation has an inherit race condition.  Be careful to ensure that
  /// Start will not be called from more than one place on the same object.  The thread
  /// will be invoked from the context which was active at the time the thread was created.
  /// </remarks>
  virtual bool Start(std::shared_ptr<Object> outstanding);

  /// <summary>
  /// Begins the core thread
  /// </summary>
  /// <param name="context">A shared pointer to the context containing this thread</param>
  /// <remarks>
  /// The default implementation of Run will simply call WaitForEvent in a loop until it's
  /// told to quit.
  /// </remarks>
  virtual void Run() = 0;

  /// <summary>
  /// Waits until the core thread is launched and then terminates
  /// </summary>
  /// <remarks>
  /// Unlike Join, this method may be invoked even if the CoreThread isn't running
  /// </remarks>
  void Wait(void) override;

  /// <summary>
  /// Timed version of Wait
  /// </summary>
  /// <returns>False if the timeout elapsed, true otherwise</returns>
  bool WaitFor(std::chrono::nanoseconds duration);

  /// <summary>
  /// Timed version of Wait
  /// </summary>
  /// <returns>False if the timeout elapsed, true otherwise</returns>
  template<class TimeType>
  bool WaitUntil(TimeType timepoint);

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
  virtual void Stop(bool graceful = false);

  /// <summary>
  /// Forces all Autowiring threads to reidentify themselves
  /// </summary>
  /// <remarks>
  /// </remarks>
  static void ForceCoreThreadReidentify(void);

  /// <returns>
  /// Returns the time when this thread was created
  /// </returns>
  /// <remarks>
  /// If the thread has not yet run, this routine returns std::steady_clock::time_point::min
  /// </remarks>
  std::chrono::steady_clock::time_point GetCreationTime(void);

  /// <summary>
  /// Obtains running time information for this thread
  /// </summary>
  void GetThreadTimes(std::chrono::milliseconds& kernelTime, std::chrono::milliseconds& userTime);
};

/// <summary>
/// Alias of CoreThread::ForceCoreThreadReidentify, provided with C-style linkage for easy invocation
/// </summary>
extern "C" void ForceCoreThreadReidentify(void);
