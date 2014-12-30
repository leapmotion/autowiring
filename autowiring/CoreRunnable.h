// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include MEMORY_HEADER
#include MUTEX_HEADER
#include CHRONO_HEADER

class Object;

class CoreRunnable {
public:
  CoreRunnable(void);
  virtual ~CoreRunnable(void);
  
private:
  // Set to true if this runnable was ever signaled to start
  bool m_wasStarted;

  // Set to true if this runnable should terminate processing
  bool m_shouldStop;

  // The outstanding count, held for as long as processing is underway
  std::shared_ptr<Object> m_outstanding;

protected:
  std::mutex m_lock;
  std::condition_variable m_cv;

  /// <returns>
  /// A reference to the current outstanding counter
  /// </returns>
  const std::shared_ptr<Object>& GetOutstanding(void) const;

  /// <summary>
  /// Method invoked from Start when this class is being asked to begin processing
  /// </summary>
  /// <returns>True if processing has started, false otherwise</returns>
  /// <remarks>
  /// This method will be called at most once.  Returning false from this method will result in an immediate invocation
  /// of the OnStop(false).
  /// </remarks>
  virtual bool OnStart(void) { return false; };

  /// <summary>
  /// Invoked by the base class when a Stop call has been made
  /// </summary>
  /// <remarks>
  /// This method will be called at most once. 
  /// </remarks>
  virtual void OnStop(bool graceful) {};

  /// <summary>
  /// Invoked by the base class when a Wait call has been made, to ensure all cleanups have happened after the Stop call
  /// </summary>
  /// <remarks>
  /// This call should not block for extended periods of time.
  /// </remarks>
  virtual void DoAdditionalWait() {};

public:
  // Accessor methods:
  bool IsRunning(void) const { return (bool)m_outstanding; }
  bool WasStarted(void) const { return m_wasStarted; }
  bool ShouldStop(void) const { return m_shouldStop; }

  /// <summary>
  /// Causes this runnable to begin processing
  /// </summary>
  /// <returns>This method always returns true</returns>
  /// <remarks>
  /// It is an error to call this routine more than once.  The passed outstanding shared pointer
  /// is used to keep tracking of number of simultaneous runnables outstanding.  This routine may
  /// be called even after Stop has been called; the caller MUST return false in this case.
  ///
  /// Callers should strongly prefer not to override Start if possible.  Instead, override OnStart and
  /// obtain an instance of the outstanding pointer via GetOutstanding
  /// </remarks>
  virtual bool Start(std::shared_ptr<Object> outstanding);

  /// <summary>
  /// Stops this runnable, optionally performing graceful cleanup if requested
  /// </summary>
  /// <remarks>
  /// On return of this method, regardless of the return value, a subsequent call to Wait is
  /// guaranteed to either return immediately, or once the thread implementation completes.
  /// </remarks>
  void Stop(bool graceful = true);

  /// <summary>
  /// Waits for this object to start running, and then stop running
  /// </summary>
  void Wait(void);

  /// <summary>
  /// Waits for this object to start running, and then stop running
  /// </summary>
  bool WaitFor(std::chrono::nanoseconds timeout);

  /// <summary>
  /// Waits for this object to start running, and then stop running
  /// </summary>
  template<typename TimeType>
  bool WaitUntil(TimeType timepoint);
};
