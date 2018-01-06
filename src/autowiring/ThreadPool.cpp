// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ThreadPool.h"
#include "DispatchQueue.h"

using namespace autowiring;

std::shared_ptr<void> ThreadPool::Start(void) {
  std::lock_guard<std::mutex> lk(m_lock);
  auto retVal = m_startToken.lock();
  if (!retVal) {
    OnStartUnsafe();

    auto pThis = shared_from_this();
    retVal.reset(
      static_cast<void*>(this),
      [pThis](void*) {
        pThis->OnStop();
      }
    );
    m_startToken = retVal;
  }
  return retVal;
}


void ThreadPool::Consume(const std::shared_ptr<DispatchQueue>& dq) {
  Submit(
    MakeDispatchThunk(
      [dq] { dq->DispatchAllEvents(); }
    )
  );
}
