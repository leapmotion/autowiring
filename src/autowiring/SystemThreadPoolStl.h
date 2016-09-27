// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include "DispatchQueue.h"
#include "SystemThreadPool.h"
#include <thread>
#include <vector>

namespace autowiring {

/// <summary>
/// Implements a thread pool using the STL as a final fallback
/// </summary>
/// <remarks>
/// This implementation avoids using std::async to achieve thread pooling because some systems
/// do not attempt to reuse threads to run operations enqueued by std::async, resulting in very
/// poor performance.
/// </remarks>
class SystemThreadPoolStl:
  public SystemThreadPool
{
public:
  SystemThreadPoolStl(void);
  ~SystemThreadPoolStl(void);

private:
  // Thunks awaiting execution
  DispatchQueue m_toBeDone;

  // The current number of outstanding workers
  std::atomic<size_t> m_outstanding{0};

  /// <summary>
  /// Creates a new worker thread to process the dispatch queue
  /// </summary>
  void AddWorkerThreadUnsafe(void);

  // ThreadPool overrides
  void OnStartUnsafe(void) override;
  void OnStop(void) override;

public:
  void SuggestThreadPoolSize(size_t nThreads) override;
  bool Submit(std::unique_ptr<DispatchThunkBase>&& thunk) override;
};

}
