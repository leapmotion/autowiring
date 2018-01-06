// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once
#include "DispatchQueue.h"
#include "SystemThreadPoolWin.hpp"
#include <Windows.h>
#include <concurrent_queue.h>

namespace autowiring {

/// <summary>
/// A thread pool that makes use of the underlying system's APIs
/// </summary>
class SystemThreadPoolWinLH:
  public SystemThreadPoolWin
{
public:
  SystemThreadPoolWinLH(void);
  ~SystemThreadPoolWinLH(void);

private:
  // Work item for single dispatchers
  PTP_WORK m_pwkDispatchRundown;
  PTP_WORK m_pwkSingle;

  // ThreadPool overrides:
  void OnStartUnsafe(void) override;
  void OnStop(void) override;

public:
  // ThreadPool overrides:
  void Consume(const std::shared_ptr<DispatchQueue>& dq) override;
  bool Submit(std::unique_ptr<DispatchThunkBase>&& thunk) override;
};

}
