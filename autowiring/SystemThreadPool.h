// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "ThreadPool.h"

namespace autowiring {

/// <summary>
/// A thread pool that makes use of the underlying system's APIs
/// </summary>
class SystemThreadPool:
  public ThreadPool
{
public:
  SystemThreadPool(void);
  ~SystemThreadPool(void);

  // Creates a new platform-specific thread pool
  static std::shared_ptr<SystemThreadPool> New(void);

  /// <summary>
  /// Makes a recommendation as to the number of worker threads that should be used to process work
  /// </summary>
  /// <remarks>
  /// Implementations are free to ignore this request.  An implementation may honor this request and
  /// then immediately change the number of pooled threads.  Passing a value of 0 does not guarantee
  /// that any threads will exit.
  ///
  /// Resizing the number of worker threads may potentially be very expensive on certain systems.
  /// This method should only be called during setup or during major stateful changes on the system.
  /// </remarks>
  virtual void SuggestThreadPoolSize(size_t nThreads) {}
};

}
