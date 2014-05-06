// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once
#include MEMORY_HEADER

class Object;

class CoreRunnable {
public:
  /// <summary>
  /// Causes this runnable to begin processing
  /// </summary>
  /// <remarks>
  /// It is an error to call this routine more than once.  The passed outstanding shared pointer
  /// is used to keep tracking of number of simultaneous runnables outstanding.
  /// </remarks>
  virtual bool Start(std::shared_ptr<Object> outstanding) = 0;

  /// <summary>
  /// Stops this runnable, optionally performing graceful cleanup if requested
  /// </summary>
  /// <remarks>
  /// On return of this method, regardless of the return value, a subsequent call to Wait is
  /// guaranteed to either return immediately, or once the thread implementation completes.
  /// </remarks>
  virtual void Stop(bool graceful) = 0;

  /// <returns>
  /// True if this runnable is currently running
  /// </returns>
  virtual bool IsRunning(void) const = 0;

  /// <summary>
  /// Waits for this object to start running, and then stop running
  /// </summary>
  virtual void Wait(void) = 0;
};
