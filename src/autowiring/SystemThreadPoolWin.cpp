// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "SystemThreadPoolWin.h"
#include "DispatchThunk.h"

using namespace autowiring;

SystemThreadPoolWin::SystemThreadPoolWin(void) 
{
}

SystemThreadPoolWin::~SystemThreadPoolWin(void)
{
  if (m_pwkSingle)
    CloseThreadpoolWork(m_pwkSingle);
  if (m_pwkDispatchRundown)
    CloseThreadpoolWork(m_pwkDispatchRundown);
}

std::shared_ptr<SystemThreadPool> SystemThreadPool::New(void)
{
  return std::make_shared<SystemThreadPoolWin>();
}

void SystemThreadPoolWin::OnStartUnsafe(void) {
  m_pwkDispatchRundown = CreateThreadpoolWork(
    [](PTP_CALLBACK_INSTANCE Instance, void* Context, PTP_WORK Work) {
      auto* pThis = static_cast<SystemThreadPoolWin*>(Context);

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

  m_pwkSingle = CreateThreadpoolWork(
    [](PTP_CALLBACK_INSTANCE Instance, void* Context, PTP_WORK Work) {
      // Spin down our dispatch queue until it is empty:
      static_cast<SystemThreadPoolWin*>(Context)->m_toBeDone.DispatchAllEvents();
    },
    this,
    nullptr
  );
}

void SystemThreadPoolWin::OnStop(void) {
  m_toBeDone.Abort();

  std::lock_guard<std::mutex> lk(m_lock);
  CloseThreadpoolWork(m_pwkSingle);
  m_pwkSingle = nullptr;
  CloseThreadpoolWork(m_pwkDispatchRundown);
  m_pwkDispatchRundown = nullptr;
}

void SystemThreadPoolWin::Consume(std::shared_ptr<DispatchQueue> dq)
{
  // Append the entry and then signal the rundown queue that there is work to be done
  std::lock_guard<std::mutex> lk(m_lock);
  m_rundownTargets.push(dq);
  if (m_pwkDispatchRundown)
    SubmitThreadpoolWork(m_pwkDispatchRundown);
}

bool SystemThreadPoolWin::Submit(std::unique_ptr<DispatchThunkBase>&& thunk)
{
  std::lock_guard<std::mutex> lk(m_lock);
  m_toBeDone.AddExisting(std::move(thunk));
  if (m_pwkSingle)
    SubmitThreadpoolWork(m_pwkSingle);
  return true;
}
