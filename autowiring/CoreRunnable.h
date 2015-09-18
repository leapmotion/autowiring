// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include CHRONO_HEADER
#include MEMORY_HEADER
#include MUTEX_HEADER

class CoreObject;

/// <summary>
/// Provides the interface for threads that should receive start and stop notifications in a context
/// </summary>
/// <remarks>
/// Users of Autowiring will typically use BasicThread or CoreThread instead of
/// extending this class.
/// </remarks>
class CoreRunnable {
public:
  CoreRunnable(void);
  virtual ~CoreRunnable(void);
  
private:
  // Set to true if this runnable was ever signaled to start
  bool m_wasStarted = false;

  // Set to true if this runnable should terminate processing
  bool m_shouldStop = false;

  // The outstanding count, held for as long as processing is underway
  std::shared_ptr<CoreObject> m_outstanding;

protected:
  std::mutex m_lock;
  std::condition_variable m_cv;

  /// <returns>
  /// A reference to the current outstanding counter
  /// </returns>
  const std::shared_ptr<CoreObject>& GetOutstanding(void) const;

  /// <summary>
  /// Invoked by the Start() method. Override this method to perform any needed setup
  /// </summary>
  /// <returns>
  /// True if processing has started, false otherwise. When overriding, returning false will shut down the
  /// runnable immediately.
  /// </returns>
  /// <remarks>
  /// This method will be called at most once.  Returning false from this method will result in an immediate
  /// invocation of OnStop(false).
  /// </remarks>
  virtual bool OnStart(void) { return false; };

  /// <summary>
  /// Invoked by the base class Stop() method. Override this method to perform any needed cleanup.
  /// </summary>
  /// <param name="graceful">
  /// Specifies whether the runnable should stop normally or whether it should exit as quickly as possible.
  /// </param>
  /// <remarks>
  /// This method will be called at most once from a passive level.
  /// </remarks>
  virtual void OnStop(bool graceful) {};

  /// <summary>
  /// Invoked just before control is returned to the user.
  /// </summary>
  /// <param name="timeout">The maximum amount of time to wait</param>
  /// <returns>True if the wait succeeded, false if a timeout occurred</returns>
  /// <remarks>
  /// This virtual method provides implementors with a way to add further constraints to the wait operation
  /// beyond the condition variable held internally by this CoreRunnable.
  ///
  /// This method must return true if the timeout is indefinite.
  /// </remarks>
  virtual bool DoAdditionalWait(std::chrono::nanoseconds timeout) { return true; }

  /// <summary>
  /// Untimed variant of DoAdditionalWait
  /// </summary>
  virtual void DoAdditionalWait(void) { }

public:
  // Accessor methods:
  /// Reports whether this runnable is currently running.
  bool IsRunning(void) const { return (bool)m_outstanding; }
  /// Reports whether this runnable was ever started.
  bool WasStarted(void) const { return m_wasStarted; }
  /// Reports whether this runnable should stop.
  bool ShouldStop(void) const { return m_shouldStop; }

  /// <summary>
  /// Causes this runnable to begin processing.
  /// </summary>
  /// <returns>This method always returns true.</returns>
  /// <remarks>
  /// It is an error to call this routine more than once.  The passed outstanding shared pointer
  /// is used to keep tracking of number of simultaneous runnables outstanding.  This routine may
  /// be called even after Stop has been called; the caller MUST return false in this case.
  ///
  /// Callers should strongly prefer not to override Start if possible.  Instead, override OnStart and
  /// obtain an instance of the outstanding pointer via GetOutstanding
  /// </remarks>
  virtual bool Start(std::shared_ptr<CoreObject> outstanding);

  /// <summary>
  /// Stops this runnable.
  /// </summary>
  /// <remarks>
  /// Calls OnStop(), forwarding the graceful parameter.
  ///
  /// On return of this method, regardless of the return value, a subsequent call to Wait is
  /// guaranteed to either return immediately, or once the thread implementation completes.
  /// </remarks>
  void Stop(bool graceful = true);

  /// <summary>
  /// Sleeps this thread for the specified duration.
  /// </summary>
  /// <returns>False if the thread was terminated before the timeout elapsed.</returns>
  bool ThreadSleep(std::chrono::nanoseconds timeout);

  /// <summary>
  /// Waits indefinitely. Returns when this runnable stops.
  /// </summary>
  void Wait(void);

  /// <summary>
  /// Waits for the specified amount of time.
  /// </summary>
  bool WaitFor(std::chrono::nanoseconds timeout);

  /// <summary>
  /// Waits until the specified time.
  /// </summary>
  /// <param name="timepoint">A std::chrono type representing a
  /// duration or future point in time.</param>
  template<typename TimeType>
  bool WaitUntil(TimeType timepoint);
};
