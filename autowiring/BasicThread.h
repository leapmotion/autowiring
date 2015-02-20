// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "ContextMember.h"
#include "CoreRunnable.h"
#include CHRONO_HEADER
#include FUNCTIONAL_HEADER
#include MEMORY_HEADER
#include MUTEX_HEADER

struct BasicThreadStateBlock;
class BasicThread;
class CoreContext;

/// \internal (until ElevatePriority feature is implmented on all platforms)
/// <summary>
/// Thread priority classifications from low to high.
/// </summary>
/// <remarks>
/// Use the ThreadPriority enumeration with the BasicThread::ElevatePriority class
/// to raise the priority of a thread when needed.
/// </remarks>
enum class ThreadPriority {
  /// This is the default thread priority. It is treated as a value lower than any of the
  /// other priorities so that an initial request to elevate can alter the OS-supplied default
  /// without causing the contention problems resulting from a high-priority operation
  /// transitioning to a low-priority state.
  Default,

  Idle, ///< Lower
  Lowest, ///< Low
  BelowNormal, ///< Below normal
  Normal, ///< Normal
  AboveNormal, ///< Above normal
  Highest, ///< High
  TimeCritical, ///< Critical

  /// This is a special case for multimedia applications.  Some operating systems, like Windows,
  /// can provide additional scheduling guarantees to applications which declare themselves as
  /// multimedia-intensive in nature.  For other systems, Multimedia is identical to TimeCritical.
  Multimedia
};

/// <summary>
/// An abstract class for creating a thread with a single Run method.
/// </summary>
/// <remarks>
/// This is an abstract class that has a single Run() method for implementation by
/// derived classes.  A BasicThread instance remains in the context as long as the thread is
/// running, even if there are no other references to it.
///
/// To create a thread, extend BasicThread and implement the desired Run() method.
/// To start the thread, wire your thread object to a context and Initiate() the context.
/// When the SignalShutown() or SignalTermination() methods of the owning context
/// are called, or you call the BasicThread::OnStop() function, the ShouldStop()
/// function will evaluate to true. It is your Run() implementation's responsibility to
/// terminate when ShouldStop() becomes true.
///
/// The following example prints out the sequence of prime numbers until the context
/// signals that threads should shutdown:
///
/// \include snippets/BasicThread_Class.txt
/// </remarks>
class BasicThread:
  public ContextMember,
  public virtual CoreRunnable
{
public:
  /// Creates a BasicThread object.
  /// <param name="pName">An optional name for this thread. The name is visible in some debuggers.</param>
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

  // The current thread priority
  ThreadPriority m_priority;

  /// <summary>
  /// Assigns a name to the thread, displayed in debuggers.
  /// </summary>
  /// <remarks>
  /// Some platforms allow the assignment of a thread name for identifying threads shown in a debugger
  /// window.  This method provides a platform-independent way of doing setting this name for the current
  /// thread.
  /// </remarks>
  void SetCurrentThreadName(void) const;

private:
  /// Only implemented on Windows (as of version 0.4.1).
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
  /// Recovers a general lock used to synchronize entities in this thread internally.
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
  virtual void DoRun(std::shared_ptr<CoreObject>&& refTracker);

  /// <summary>
  /// Performs all cleanup operations that must take place after DoRun()
  /// </summary>
  /// <param name="pusher">The last reference to the enclosing context held by this thread</param>
  virtual void DoRunLoopCleanup(std::shared_ptr<CoreContext>&& ctxt, std::shared_ptr<CoreObject>&& refTracker);

  void DEPRECATED(Ready(void) const, "Do not call this method, the concept of thread readiness is now deprecated") {}

  /// \internal Only implemented on Windows (as of 0.4.1).
  /// <summary>
  /// Boosts thread priority while an instance of this type exists.
  /// </summary>
  /// <remarks>
  /// The thread priority is changed when the requested priority is higher than the
  /// current priority.  The destructor automatically restores the previous priority.
  /// This class cannot be moved or copied, in order to guarantee proper RAII.
  /// </remarks>
  class ElevatePriority {
  public:
    /// Creating an ElevatePriority object as a member of a BasicThread instance
    /// elevates the priority of the thread if the specified priority is higher
    /// then the current thread priority. Destroy this ElevatePriority instance
    /// to restore the normal thread priority.
    ElevatePriority(ElevatePriority&) = delete;

    /// Elevates the priority of a BasicThread instance if the specified priority is higher
    /// then the current thread priority. Destroy this ElevatePriority instance
    /// to restore the normal thread priority.
    ElevatePriority(BasicThread& thread, ThreadPriority priority) :
      m_oldPriority(thread.m_priority),
      m_thread(thread)
    {
      // Elevate if the new level is higher than the old level:
      if(priority > m_oldPriority)
        m_thread.SetThreadPriority(priority);
    }

    /// Destroying this object returns the thread to its previous priority
    /// level.
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
  /// Waits until the specified lambda function returns true or the thread shuts down.
  /// </summary>
  /// <remarks>
  /// The lambda function is called repeatedly until it evaluates to true.
  /// </remarks>
  void WaitForStateUpdate(const std::function<bool()>& fn);

  /// <summary>
  /// Obtains a mutex, invokes the specified lambda function, and then updates
  /// the basic thread's state condition.
  /// </summary>
  void PerformStatusUpdate(const std::function<void()>& fn);

  /// <summary>
  /// Sleeps this thread for the specified duration.
  /// </summary>
  /// <returns>False if the thread was terminated before the timeout elapsed.</returns>
  /// <remarks>
  /// Events are dispatched by this method while the sleep is taking place, which makes this
  /// method similar to an alertable wait on Windows.  Callers are cautioned against holding
  /// locks while calling this method; if this is done, a deadlock could result.
  ///
  /// Callers should not invoke this method outside of this thread's thread context, or an
  /// interruption exception could result.
  /// </remarks>
  bool ThreadSleep(std::chrono::nanoseconds timeout);

  /// <summary>
  /// Causes a new thread to be created in which the Run method will be invoked
  /// </summary>
  /// <returns>True to indicate that the thread was started successfully, false if it was already started or cancelled</returns>
  /// <remarks>
  /// Note that this operation has an inherit race condition.  Be careful to ensure that
  /// Start will not be called from more than one place on the same object.  The thread
  /// will be invoked from the context which was active at the time the thread was created.
  /// </remarks>
  bool OnStart() override;

  void OnStop(bool graceful) override;

  void DoAdditionalWait() override;

public:
  /// <summary>
  /// Begins thread execution.
  /// </summary>
  /// <remarks>
  /// Implement this function to perform the work to be done by this thread. Your
  /// Run() function implementation must exit when ShouldStop() becomes true.
  ///
  /// BasicThreads can be stopped gracefully or immediately. Override OnStop(bool graceful)
  /// to implement different behavior for these two shutdown modes. For example,
  /// if the shutdown mode is immediate, you might abort any actions in progress,
  /// but if the mode is graceful, you might take time to save files, etc. This
  /// behavior is application defined. Autowiring does not impose its own requirements.
  /// You can also perform custom shutdown cleanup in an OnStop() implementation.
  ///
  /// The default implementation of Run will simply call WaitForEvent in a loop until it's
  /// told to quit.
  /// </remarks>
  virtual void Run() = 0;

  /// <summary>
  /// Invoked by the base class Stop() method. Implement this method to perform any needed cleanup.
  /// </summary>
  /// <remarks>
  /// Do not perform any time-consuming operations in this callback; the method may be called
  /// from a time-sensitive context and unacceptable system performance could result if long-duration
  /// operations are undertaken here.
  ///
  /// Override the OnStop(bool graceful) version of this function to handle immediate vs. graceful shutdown
  /// cleanup.
  /// </remarks>
  virtual void OnStop(void) {}

  /// <summary>
  /// Forces all Autowiring threads to reidentify themselves.
  /// </summary>
  /// <remarks>
  /// Calls SetThreadName() for each thread.
  /// </remarks>
  static void ForceCoreThreadReidentify(void);

  /// <summary>
  /// The thread creation time.
  /// </summary>
  /// <returns>
  /// Returns the time when this thread was created.
  /// </returns>
  /// <remarks>
  /// If the thread has not yet run, this routine returns std::steady_clock::time_point::min
  /// </remarks>
  std::chrono::steady_clock::time_point GetCreationTime(void);

  /// <summary>
  /// Reports kernel and user mode running times for this thread.
  /// </summary>
  /// <remarks>
  /// This function is intended for debugging and performance reporting. It is used
  /// by the AutoNet server.
  ///
  /// \include snippets/BasicThread_GetRunningTime.txt
  /// </remarks>
  void GetThreadTimes(std::chrono::milliseconds& kernelTime, std::chrono::milliseconds& userTime);
};

/// <summary>
/// Alias of CoreThread::ForceCoreThreadReidentify, provided with C-style linkage for easy invocation.
/// </summary>
extern "C" void ForceCoreThreadReidentify(void);
