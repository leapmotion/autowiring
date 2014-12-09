// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include MEMORY_HEADER

class Object;

class CoreRunnable {
public:
  CoreRunnable(void);
  virtual ~CoreRunnable(void);
  
private:
  // Set to true if this runnable was ever signalled to start
  bool m_wasStarted;

  // The outstanding count, held for as long as processing is underway
  std::shared_ptr<Object> m_outstanding;

  // Set to true if this runnable should terminate processing
  bool m_shouldStop;

protected:
  std::mutex m_lock;
  std::condition_variable m_cv;

  /// <summary>
  /// Method invoked from Start when this class is being asked to begin processing
  /// </summary>
  /// <returns>True if processing has started, false otherwise</returns>
  /// <remarks>
  /// This method will be called at most once.
  /// </remarks>
  virtual bool DoStart(void) = 0;

  /// <summary>
  /// Invoked by the base class when a Stop call has been made
  /// </summary>
  /// <remarks>
  /// This method will be called at most once, and may potentially be called even 
  /// </remarks>
  virtual void OnStop(bool graceful) = 0;

public:
  // Accessor methods:
  bool IsRunning(void) const { return (bool)m_outstanding; }
  bool WasStarted(void) const { return m_wasStarted; }
  bool ShouldStop(void) const { return m_shouldStop; }

  /// <summary>
  /// Causes this runnable to begin processing
  /// </summary>
  /// <returns>True if this call resulted in a successful start the first time, false in all other cases</returns>
  /// <remarks>
  /// It is an error to call this routine more than once.  The passed outstanding shared pointer
  /// is used to keep tracking of number of simultaneous runnables outstanding.  This routine may
  /// be called even after Stop has been called; the caller MUST return false in this case.
  /// </remarks>
  void Start(std::shared_ptr<Object> outstanding);

  /// <summary>
  /// Stops this runnable, optionally performing graceful cleanup if requested
  /// </summary>
  /// <remarks>
  /// On return of this method, regardless of the return value, a subsequent call to Wait is
  /// guaranteed to either return immediately, or once the thread implementation completes.
  /// </remarks>
  virtual void Stop(bool graceful);

  /// <summary>
  /// Waits for this object to start running, and then stop running
  /// </summary>
  void Wait(void);

  /// <summary>
  /// Waits for this object to start running, and then stop running
  /// </summary>
  void WaitFor(std::chrono::nanoseconds timeout);
};
