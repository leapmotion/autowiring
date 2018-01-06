// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "SystemThreadPoolStl.h"
#include "at_exit.h"

using namespace autowiring;

SystemThreadPoolStl::SystemThreadPoolStl(void):
  m_toBeDone(~0)
{}

SystemThreadPoolStl::~SystemThreadPoolStl(void)
{}

// Used on non-MSVC platforms, but we still want to be able to test the rest of these pool
// behaviors on MSVC
#ifndef _MSC_VER
std::shared_ptr<SystemThreadPool> SystemThreadPool::New(void) {
  return std::make_shared<SystemThreadPoolStl>();
}
#endif

void SystemThreadPoolStl::AddWorkerThreadUnsafe(void) {
  auto pThis = shared_from_this();
  std::thread t([this, pThis] {
    auto clear = MakeAtExit([&] { m_outstanding--; });
    try {
      for (;;)
        m_toBeDone.WaitForEvent();
    }
    catch (dispatch_aborted_exception&) {
      // Dispatch aborted exception, back out
    }
    catch (...) {
      // Unknown, we have nowhere to report this exception, silently fail
    }
  });
  t.detach();
  m_outstanding++;
}

void SystemThreadPoolStl::OnStartUnsafe(void) {
  if (m_outstanding)
    // Do nothing if the pool size was already set by someone else
    return;

  // TODO:  Set this number according to std::thread::hardware_concurrency (or
  // get_nprocs() on gcc, to retain libstdc++ backwards-compatibility).  This can't
  // be done right now due to the fact that DispatchQueue has terrible concurrency
  // performance.
  while (m_outstanding < 2)
    AddWorkerThreadUnsafe();
}

void SystemThreadPoolStl::OnStop(void) {
  m_toBeDone += [this] { m_toBeDone.Abort(); };
}

void SystemThreadPoolStl::SuggestThreadPoolSize(size_t nThreads) {
  std::lock_guard<std::mutex> lk(m_lock);
  while (m_outstanding < nThreads)
    AddWorkerThreadUnsafe();
}

bool SystemThreadPoolStl::Submit(std::unique_ptr<DispatchThunkBase>&& thunk) {
  // Add some more work
  m_toBeDone.AddExisting(std::move(thunk));

  // If we don't have anyone to do work, we need to wake someone up:
  std::lock_guard<std::mutex> lk(m_lock);
  if (!m_outstanding && !m_startToken.expired())
    AddWorkerThreadUnsafe();

  return false;
}
