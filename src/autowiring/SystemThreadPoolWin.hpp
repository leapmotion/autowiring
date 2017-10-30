// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include "DispatchQueue.h"
#include "SystemThreadPool.h"
#include <Windows.h>
#include <concurrent_queue.h>

namespace autowiring {

  namespace detail {
    extern const decltype(&CreateThreadpoolWork) g_CreateThreadpoolWork;
    extern const decltype(&CloseThreadpoolWork) g_CloseThreadpoolWork;
    extern const decltype(&SubmitThreadpoolWork) g_SubmitThreadpoolWork;
  }

/// <summary>
/// Abstract base class used by Windows XP and LH compatibility layers
/// </summary>
class SystemThreadPoolWin:
  public SystemThreadPool
{
public:
  SystemThreadPoolWin(void);
  ~SystemThreadPoolWin(void);

protected:
  // Vector of dispathc queues that need to be run down
  concurrency::concurrent_queue<std::shared_ptr<DispatchQueue>> m_rundownTargets;

  // Our own internal dispatch queue containing items to be executed:
  DispatchQueue m_toBeDone;

  // ThreadPool overrides:
  void OnStop(void) override;
};

}
