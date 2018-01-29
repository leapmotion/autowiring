// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once
#include "ContextMember.h"
#include "CoreRunnable.h"
#include CHRONO_HEADER
#include FUNCTIONAL_HEADER
#include MEMORY_HEADER
#include MUTEX_HEADER
#include THREAD_HEADER
#include <atomic>

class BasicThread;
class CoreContext;

namespace autowiring {
  struct BasicThreadStateBlock;
}

/// <summary>
/// Thread priority classifications from low to high.
/// </summary>
/// <remarks>
/// Use the ThreadPriority enumeration with BasicThread::BasicThread,
/// BasicThread::SetThreadPriority, or the BasicThread::ElevatePriority class
/// to adjust the priority of a thread when needed.
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
/// Scheduling policies.
/// </summary>
/// <remarks>
/// Use the SchedulingPolicy enumeration with BasicThread::BasicThread,
/// BasicThread::SetThreadPriority, BasicThread::SetSchedulingPolicy, or
/// BasicThread::SetDefaultSchedulingPolicy. Not all platforms support the scheduling policy.
/// </remarks>
enum class SchedulingPolicy {
  /// This is the default scheduling policy used by all new threads. It is a standard
  /// round-robin policy. It will also run with a lower priority than any of the realtime
  /// policies.
  StandardRoundRobin,

  /// For time-critical applications, realtime policies may be used:
  RealtimeFIFO, ///< A realtime first-in, first-out policy
  RealtimeRoundRobin ///< A realtime round-robin policy
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
  public CoreRunnable
{
public:
  /// Creates a BasicThread object.
  /// <param name="pName">An optional name for this thread. The name is visible in some debuggers.</param>
  BasicThread(const char* pName = nullptr);
  /// Creates a BasicThread object with the specified thread priority and name.
  /// <param name="threadPriority">A priority for this thread.</param>
  /// <param name="pName">An optional name for this thread. The name is visible in some debuggers.</param>
  BasicThread(ThreadPriority threadPriority, const char* pName = nullptr);
  /// Creates a BasicThread object with the specified thread priority, scheduling policy, and name.
  /// <param name="threadPriority">A priority for this thread.</param>
  /// <param name="schedPolicy">A scheduling policy for this thread.</param>
  /// <param name="pName">An optional name for this thread. The name is visible in some debuggers.</param>
  BasicThread(ThreadPriority threadPriority, SchedulingPolicy schedPolicy, const char* pName = nullptr);
  virtual ~BasicThread(void);

  /// <summary>
  /// Set the default scheduling policy to use for newly created threads
  /// </summary>
  static SchedulingPolicy SetDefaultSchedulingPolicy(SchedulingPolicy schedPolicy);

  /// <summary>
  /// Get the current default scheduling policy
  /// </summary>
  static SchedulingPolicy GetDefaultSchedulingPolicy(void);

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
    ElevatePriority(BasicThread& thread, ThreadPriority threadPriority) :
      m_thread(thread),
      // Elevate if the new level is greater than the current level:
      m_oldPriority(thread.ElevateThreadPriority(threadPriority))
    {
    }

    /// Destroying this object returns the thread to its previous priority
    /// level.
    ~ElevatePriority(void) {
      // De-elevate if the old level is lower than the current level:
      m_thread.DeelevateThreadPriority(m_oldPriority);
    }

  private:
    BasicThread& m_thread;
    ThreadPriority m_oldPriority;
  };

protected:
  /// The default scheduling policy used when creating threads (defaults to StandardRoundRobin)
  static std::atomic<SchedulingPolicy> s_schedulingPolicy;

  // Internally held thread status block.  This has to be a shared pointer because we need to signal
  // the held state condition after releasing all shared pointers to ourselves, and this could mean
  // we're actually signalling this event after we free ourselves.
  const std::shared_ptr<autowiring::BasicThreadStateBlock> m_state;

  // Flag indicating that this thread was started at some point
  bool m_wasStarted = false;

  // Flag indicating that we need to stop right now
  bool m_stop = false;

  // Run condition:
  bool m_running = false;

  /// <summary>
  /// Assigns a name to the thread, displayed in debuggers.
  /// </summary>
  /// <remarks>
  /// Some platforms allow the assignment of a thread name for identifying threads shown in a debugger
  /// window.  This method provides a platform-independent way of doing setting this name for the current
  /// thread.
  /// </remarks>
  void SetCurrentThreadName(void) const;

  /// <summary>
  /// Sets the thread priority of this thread
  /// </summary>
  /// <remarks>
  /// This method may be called while the thread is running, or before it starts to run.  If it is
  /// invoked before the thread starts to run, the thread will take on the specified priority when
  /// it is started.
  /// </remarks>
  /// <returns>
  /// The previous thread priority
  /// </returns>
  ThreadPriority SetThreadPriority(ThreadPriority threadPriority);

  /// <summary>
  /// Sets the thread priority and scheduling policy of this thread
  /// </summary>
  /// <remarks>
  /// This method may be called while the thread is running, or before it starts to run.  If it is
  /// invoked before the thread starts to run, the thread will take on the specified priority and
  /// scheduling policy when it is started.
  /// </remarks>
  /// <returns>
  /// The previous thread priority
  /// </returns>
  ThreadPriority SetThreadPriority(ThreadPriority threadPriority, SchedulingPolicy schedPolicy);

  /// <summary>
  /// Sets the scheduling policy of this thread
  /// </summary>
  /// <remarks>
  /// This method may be called while the thread is running, or before it starts to run.  If it is
  /// invoked before the thread starts to run, the thread will take on the scheduling policy when
  /// it is started.
  /// </remarks>
  /// <returns>
  /// The previous thread priority
  /// </returns>
  SchedulingPolicy SetSchedulingPolicy(SchedulingPolicy schedPolicy);

  /// <summary>
  /// Sets the thread priority of this thread only if it elevates the priority
  /// </summary>
  /// <remarks>
  /// This method may be called while the thread is running, or before it starts to run.  If it is
  /// invoked before the thread starts to run, the thread will take on the specified priority when
  /// it is started.
  /// </remarks>
  /// <returns>
  /// The previous thread priority
  /// </returns>
  ThreadPriority ElevateThreadPriority(ThreadPriority threadPriority);

  /// <summary>
  /// Sets the thread priority of this thread only if it de-elevates the priority
  /// </summary>
  /// <remarks>
  /// This method may be called while the thread is running, or before it starts to run.  If it is
  /// invoked before the thread starts to run, the thread will take on the specified priority when
  /// it is started.
  /// </remarks>
  /// <returns>
  /// The previous thread priority
  /// </returns>
  ThreadPriority DeelevateThreadPriority(ThreadPriority threadPriority);

  /// <summary>
  /// Low-level function to set the thread priority and scheduling policy
  /// </summary>
  static void SetThreadPriority(const std::thread::native_handle_type& handle, ThreadPriority threadPriority, SchedulingPolicy schedPolicy);

  /// <summary>
  /// Recovers a general lock used to synchronize entities in this thread internally.
  /// </summary>
  std::mutex& GetLock(void) const;

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
  /// <param name="ctxt">The last reference to the enclosing context held by this thread</param>
  /// <param name="refTracker">A reference tracker held for as long as the cleanup operation is incomplete</param>
  virtual void DoRunLoopCleanup(std::shared_ptr<CoreContext>&& ctxt, std::shared_ptr<CoreObject>&& refTracker);

  /// <summary>
  /// Waits until the specified lambda function returns true or the thread shuts down.
  /// </summary>
  /// <remarks>
  /// The lambda function is called repeatedly until it evaluates to true.
  /// </remarks>
  void WaitForStateUpdate(const std::function<bool()>& fn) const;

  /// <summary>
  /// Obtains a mutex, invokes the specified lambda function, and then updates
  /// the basic thread's state condition.
  /// </summary>
  void PerformStatusUpdate(const std::function<void()>& fn) const;

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

  void DoAdditionalWait(void) override;
  bool DoAdditionalWait(std::chrono::nanoseconds timeout) override;

public:
  /// <returns>
  /// A void pointer to the underlying std::thread
  /// </returns>
  std::shared_ptr<void> GetThread(void) const;

  /// <returns>
  /// The current thread priority
  /// </returns>
  ThreadPriority GetThreadPriority(void);

  /// <returns>
  /// The current scheduling policy
  /// </returns>
  SchedulingPolicy GetSchedulingPolicy(void);

  /// <returns>
  /// True if this thread has transitioned to a completed state
  /// </returns>
  bool IsCompleted(void) const;

  /// <summary>
  /// Adds a function object which will be called when this BasicThread stops running or is destroyed
  /// </summary>
  /// <remarks>
  /// The listener is invoked before the destruction of this BasicThread and also immediately after the
  /// BasicThread instance has transitioned to the Stopped state.
  ///
  /// Users who attach a teardown listener MUST NOT attempt to invoke any methods not defined by
  /// BasicThread, and MUST NOT attempt to invoke any non-final virtual functions available here.  The
  /// object itself may be partially destroyed by the time the listener is invoked, and virtual methods
  /// may not have the expected behavior.
  ///
  /// It is guaranteed to be safe to call any non-virtual method defined by BasicThread from a teardown
  /// listener.
  /// </remarks>
  template<class Fx>
  void AddTeardownListener(Fx&& listener) {
    return TeardownNotifier::AddTeardownListener(std::forward<Fx&&>(listener));
  }

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

  /// <returns>
  /// True if the calling thread is the main thread
  /// </returns>
  static bool IsMainThread(void);

private:
  /// <summary>
  /// Update the thread priority to its current value
  /// </summary>
  /// <remarks>
  /// Useful for re-setting the priority of a thread that may have be specified before it was started.
  /// </remarks>
  void UpdateThreadPriority(std::unique_lock<std::mutex>&& lock);
};

/// <summary>
/// Alias of CoreThread::ForceCoreThreadReidentify, provided with C-style linkage for easy invocation.
/// </summary>
extern "C" void ForceCoreThreadReidentify(void);
