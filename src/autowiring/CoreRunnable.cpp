// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "CoreRunnable.h"

// Explicit instantiation of supported time point types:
template<> bool CoreRunnable::WaitUntil(std::chrono::steady_clock::time_point);
template<> bool CoreRunnable::WaitUntil(std::chrono::system_clock::time_point);

CoreRunnable::CoreRunnable(void):
  m_wasStarted(false),
  m_shouldStop(false)
{}

CoreRunnable::~CoreRunnable(void) {}

void CoreRunnable::Start(std::shared_ptr<Object> outstanding) {
  std::lock_guard<std::mutex> lk(m_lock);
  if(m_wasStarted || m_outstanding || m_shouldStop) {
    // We have already been started or stopped, end here
    return;
  }

  m_wasStarted = true;
  m_outstanding = outstanding;
  if(!DoStart()) {
    m_shouldStop = true;
    m_outstanding.reset();
    return;
  }
}

void CoreRunnable::Stop(bool graceful) {
  if(m_shouldStop) {
    // We were never started or have already stopped, end here
    return;
  } else if (!m_wasStarted) {
    // We were never started, and now we never will be.
    m_shouldStop = true;
    OnStop(graceful);
    return;
  }

  std::shared_ptr<Object> outstanding;

  m_shouldStop = true;
  OnStop(graceful);

  std::lock_guard<std::mutex> lk(m_lock);
  outstanding.swap(m_outstanding);
  m_cv.notify_all();
}

void CoreRunnable::Wait(void) {
  std::unique_lock<std::mutex> lk(m_lock);
  m_cv.wait(lk, [this](){ return ShouldStop() && !IsRunning(); });
  DoAdditionalWait();
}

bool CoreRunnable::WaitFor(std::chrono::nanoseconds timeout) {
  std::unique_lock<std::mutex> lk(m_lock);
  if (m_cv.wait_for(lk, timeout, [this](){ return ShouldStop() && !IsRunning(); })) {
    DoAdditionalWait();
    return true;
  }
  return false;
}

template<typename TimeType>
bool CoreRunnable::WaitUntil(TimeType timepoint) {
  std::unique_lock<std::mutex> lk(m_lock);
  if (m_cv.wait_until(lk, timepoint, [this](){ return ShouldStop() && !IsRunning(); })) {
    DoAdditionalWait();
    return true;
  }
  return false;
}
