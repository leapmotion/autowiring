// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "SystemThreadPoolWinXP.hpp"
#include "DispatchThunk.h"

using namespace autowiring;

SystemThreadPoolWinXP::SystemThreadPoolWinXP(void) {}

SystemThreadPoolWinXP::~SystemThreadPoolWinXP(void) {}

void SystemThreadPoolWinXP::Consume(const std::shared_ptr<DispatchQueue>& dq)
{
  // Append the entry and then signal the rundown queue that there is work to be done
  std::lock_guard<std::mutex> lk(m_lock);
  m_rundownTargets.push(dq);
  QueueUserWorkItem(
    [](void* Context) {
      auto* pThis = static_cast<SystemThreadPoolWinXP*>(Context);

      // Grab an arbitrary dispatch queue from the set of pending queues:
      std::shared_ptr<DispatchQueue> dqEntry;
      if (!pThis->m_rundownTargets.try_pop(dqEntry))
        // Short circuit, someone must have got to it already
        return 0UL;

      // Now just dispatch everything in order:
      dqEntry->DispatchAllEvents();
      return 0UL;
    },
    this,
    WT_EXECUTEDEFAULT
  );
}

bool SystemThreadPoolWinXP::Submit(std::unique_ptr<DispatchThunkBase>&& thunk)
{
  std::lock_guard<std::mutex> lk(m_lock);
  m_toBeDone.AddExisting(std::move(thunk));
  QueueUserWorkItem(
    [](void* Context) {
      // Spin down our dispatch queue until it is empty:
      static_cast<SystemThreadPoolWinXP*>(Context)->m_toBeDone.DispatchAllEvents();
      return 0UL;
    },
    this,
    WT_EXECUTEDEFAULT
  );
  return true;
}
