// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include "DispatchQueue.h"
#include "ThreadPool.h"

namespace autowiring {

class ManualThreadPool;

struct ThreadPoolToken {
  ThreadPoolToken(const std::shared_ptr<ManualThreadPool>& manualPool) :
    manualPool(manualPool)
  {}

  const std::shared_ptr<ManualThreadPool> manualPool;

  // True if the thread owning this token should back out
  bool cancelled = false;

  /// <summary>
  /// Causes the thread that has joined the thread pool to leave that pool
  /// </summary>
  /// <remarks>
  /// This method is idempotent.  This method may be called on a token either before or after the
  /// corresponding thread has begun participating in the pool.
  /// </remarks>
  void Leave(void);
};

/// <summary>
/// Implements a thread pool that allows callers much more direct control of execution contexts
/// </summary>
/// <remarks>
/// This thread pool implementation does not attempt to create any worker threads to handle work
/// submitted to it.  Such submissions will accumulate in the internal DispatchQueue until a
/// thread calls ManualThreadPool::Join.  At that point, the calling thread--referred to as a
/// pool participant--will be responsible for executing work associated with the queue.
/// </remarks>
class ManualThreadPool:
  public ThreadPool,
  protected DispatchQueue
{
public:
  ManualThreadPool(void);
  ~ManualThreadPool(void);

private:
  // Number of threads currently executing dispatchers
  std::atomic<size_t> m_outstanding;

  // ThreadPool overrides:
  void OnStartUnsafe(void);
  void OnStop(void);

public:
  using DispatchQueue::WakeAllWaitingThreads;

  /// <summary>
  /// Provides a shared pointer that a participating thread may use to release itself from the pool
  /// </summary>
  /// <remarks>
  /// The returned token may be used in exactly one call to Join.  Tokens should be reset when
  /// the join call returns.  Tokens may safely outlive the thread pool that issued them.
  /// </remarks>
  std::shared_ptr<ThreadPoolToken> PrepareJoin(void);

  /// <summary>
  /// Causes the caller to perform work on this thread pool until the pool is stopped
  /// </summary>
  /// <returns>False if the passed token was already cancelled when this call was made; true otherwise</returns>
  /// <param name="token">The token returned by a prior call to PrepareJoin</param>
  void Join(std::shared_ptr<ThreadPoolToken> token);

  // ThreadPool overrides:
  bool Submit(std::unique_ptr<DispatchThunkBase>&& thunk);
};

}
