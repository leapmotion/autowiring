// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "CoreRunnable.h"

// Explicit instantiation of supported time point types:
template<> bool CoreRunnable::WaitUntil(std::chrono::steady_clock::time_point);
template<> bool CoreRunnable::WaitUntil(std::chrono::system_clock::time_point);

CoreRunnable::CoreRunnable(void) {}
CoreRunnable::~CoreRunnable(void) {}

bool CoreRunnable::IsRunning(void) const {
  return (bool)m_outstanding;
}

const std::shared_ptr<CoreObject>& CoreRunnable::GetOutstanding(void) const {
  return m_outstanding;
}

bool CoreRunnable::Start(std::shared_ptr<CoreObject> outstanding) {
  std::lock_guard<std::mutex> lk(m_lock);
  if(m_wasStarted || m_outstanding || m_shouldStop)
    // We have already been started or stopped, end here
    return true;

  m_wasStarted = true;
  m_outstanding = outstanding;
  if(!OnStart()) {
    m_shouldStop = true;
    m_outstanding.reset();

    // Immediately invoke a graceless stop in response
    OnStop(false);
  }

  return true;
}

void CoreRunnable::Stop(bool graceful) {
  if (!m_shouldStop) {
    // Stop flag should be pulled high
    m_shouldStop = true;

    // Do not call this method more than once:
    OnStop(graceful);
  }

  if (m_outstanding) {
    std::shared_ptr<CoreObject> outstanding;
    std::lock_guard<std::mutex> lk(m_lock);

    // Ensure we do not invoke the outstanding count dtor while holding a lock
    outstanding.swap(m_outstanding);

    // Everything looks good now
    m_cv.notify_all();
  }
}

bool CoreRunnable::ThreadSleep(std::chrono::nanoseconds timeout) {
  std::unique_lock<std::mutex> lk(m_lock);
  return m_cv.wait_for(lk, timeout, [this] { return ShouldStop(); });
}

void CoreRunnable::Wait(void) {
  {
    std::unique_lock<std::mutex> lk(m_lock);
    m_cv.wait(lk, [this] { return ShouldStop() && !IsRunning(); });
  }
  DoAdditionalWait();
}

bool CoreRunnable::WaitFor(std::chrono::nanoseconds timeout) {
  {
    std::unique_lock<std::mutex> lk(m_lock);
    if (!m_cv.wait_for(lk, timeout, [this] { return ShouldStop() && !IsRunning(); }))
      return false;
  }
  return DoAdditionalWait(timeout);
}

template<typename TimeType>
bool CoreRunnable::WaitUntil(TimeType timepoint) {
  {
    std::unique_lock<std::mutex> lk(m_lock);
    if (!m_cv.wait_until(lk, timepoint, [this] { return ShouldStop() && !IsRunning(); }))
      return false;
  }
  return DoAdditionalWait(timepoint - TimeType::now());;
}
