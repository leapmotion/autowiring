// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <atomic>
#include <memory>
#include <mutex>

class DispatchQueue;
class DispatchThunkBase;

namespace autowiring {

/// <summary>
/// Generic interface for a thread pool
/// </summary>
class ThreadPool:
  public std::enable_shared_from_this<ThreadPool>
{
public:
  ThreadPool(void) {}
  virtual ~ThreadPool(void) {}

protected:
  // Start token:
  std::mutex m_lock;
  std::weak_ptr<void> m_startToken;

  /// <summary>
  /// Called when the thread pool is first started
  /// </summary>
  /// <remarks>
  /// This method is called at most once
  /// </remarks>
  virtual void OnStartUnsafe(void) {}

  /// <summary>
  /// Called when the thread pool is being cleaned up
  /// </summary>
  virtual void OnStop(void) {}

public:
  /// <returns>True if the thread pool is running</returns>
  bool IsStarted(void) const { return !m_startToken.expired(); }

  /// <summary>
  /// Begins processing on this thread pool
  /// </summary>
  /// <returns>A token that must be held in scope while the thread pool should be started</returns>
  /// <remarks>
  /// This method is idempotent.  Unlike CoreThread instances, a thread pool may be restarted.
  /// The returned shared pointer must be held for as long as the thread pool should be kept running.
  /// </remarks>
  virtual std::shared_ptr<void> Start(void);

  /// <summary>
  /// Causes the thread pool to call all lambdas specified on the passed DispatchQueue
  /// </summary>
  /// <remarks>
  /// The dispatchers on the DispatchQueue are executed sequentially with respect to each other.
  /// Each dispatcher is guaranteed to be destroyed before the next one is executed.  Dispatchers
  /// may potentially be executed on different threads, depending on the exact details of the
  /// ThreadPool's implementation.  There is no guarantee of timeliness of execution provided
  /// by this implementation; the queue may be emptied before this function returns in some cases,
  /// or its state may not be changed at all.  The DispatchQueue may be aborted while the pool
  /// has posession of it; this will cause the ThreadPool to abandon processing of the queue
  /// in the normal way.
  ///
  /// Once the pool has emptied the queue, it stops processing dispatchers from it.  The user
  /// must call Consume again in order to continue processing.
  ///
  /// This method is guaranteed not to block.  The default implementation captures the passed
  /// queue in a lambda and invokes Submit with this constructed lambda.
  /// </remarks>
  virtual void Consume(const std::shared_ptr<DispatchQueue>& dq);

  /// <summary>
  /// Adds the specified thunk to be executed by the thread pool at some later time
  /// </summary>
  /// <returns>True if the job was successfully accepted, false otherwise</returns>
  /// <remarks>
  /// This method may be called even if the thread pool is not currently running.  In that
  /// case, the job will be enqueued until the thread pool is started, at which time it will
  /// be submitted for execution.
  /// </remarks>
  virtual bool Submit(std::unique_ptr<DispatchThunkBase>&& thunk) = 0;
};

}