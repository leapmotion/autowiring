// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "DispatchQueue.h"
#include "SystemThreadPool.h"
#include <Windows.h>
#include <concurrent_queue.h>

namespace autowiring {

/// <summary>
/// A thread pool that makes use of the underlying system's APIs
/// </summary>
class SystemThreadPoolWin:
  public SystemThreadPool
{
public:
  SystemThreadPoolWin(void);
  ~SystemThreadPoolWin(void);

private:
  // Work item for single dispatchers
  PTP_WORK m_pwkDispatchRundown;
  PTP_WORK m_pwkSingle;

  // Vector of dispathc queues that need to be run down
  concurrency::concurrent_queue<std::shared_ptr<DispatchQueue>> m_rundownTargets;

  // Our own internal dispatch queue containing items to be executed:
  DispatchQueue m_toBeDone;

  // ThreadPool overrides:
  void OnStartUnsafe(void) override;
  void OnStop(void) override;

public:
  // ThreadPool overrides:
  void Consume(const std::shared_ptr<DispatchQueue>& dq) override;
  bool Submit(std::unique_ptr<DispatchThunkBase>&& thunk) override;
};

}
