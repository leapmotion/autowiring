// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "SystemThreadPoolWinLH.hpp"
#include "DispatchThunk.h"

using namespace autowiring;
using namespace autowiring::detail;

SystemThreadPoolWinLH::SystemThreadPoolWinLH(void) {}

SystemThreadPoolWinLH::~SystemThreadPoolWinLH(void)
{
  if (m_pwkSingle)
    g_CloseThreadpoolWork(m_pwkSingle);
  if (m_pwkDispatchRundown)
    g_CloseThreadpoolWork(m_pwkDispatchRundown);
}

void SystemThreadPoolWinLH::OnStartUnsafe(void) {
  m_pwkDispatchRundown = g_CreateThreadpoolWork(
    [](PTP_CALLBACK_INSTANCE Instance, void* Context, PTP_WORK Work) {
      auto* pThis = static_cast<SystemThreadPoolWinLH*>(Context);

      // Grab an arbitrary dispatch queue from the set of pending queues:
      std::shared_ptr<DispatchQueue> dqEntry;
      if (!pThis->m_rundownTargets.try_pop(dqEntry))
        // Short circuit, someone must have got to it already
        return;

      // Now just dispatch everything in order:
      dqEntry->DispatchAllEvents();
    },
    this,
    nullptr
  );

  m_pwkSingle = g_CreateThreadpoolWork(
    [](PTP_CALLBACK_INSTANCE Instance, void* Context, PTP_WORK Work) {
      // Spin down our dispatch queue until it is empty:
      static_cast<SystemThreadPoolWinLH*>(Context)->m_toBeDone.DispatchAllEvents();
    },
    this,
    nullptr
  );
}

void SystemThreadPoolWinLH::OnStop(void) {
  SystemThreadPoolWin::OnStop();

  std::lock_guard<std::mutex> lk(m_lock);
  g_CloseThreadpoolWork(m_pwkSingle);
  m_pwkSingle = nullptr;
  g_CloseThreadpoolWork(m_pwkDispatchRundown);
  m_pwkDispatchRundown = nullptr;
}

void SystemThreadPoolWinLH::Consume(const std::shared_ptr<DispatchQueue>& dq)
{
  // Append the entry and then signal the rundown queue that there is work to be done
  std::lock_guard<std::mutex> lk(m_lock);
  m_rundownTargets.push(dq);
  if (m_pwkDispatchRundown)
    g_SubmitThreadpoolWork(m_pwkDispatchRundown);
}

bool SystemThreadPoolWinLH::Submit(std::unique_ptr<DispatchThunkBase>&& thunk)
{
  std::lock_guard<std::mutex> lk(m_lock);
  m_toBeDone.AddExisting(std::move(thunk));
  if (m_pwkSingle)
    g_SubmitThreadpoolWork(m_pwkSingle);
  return true;
}
