// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "DispatchQueue.h"
#include "at_exit.h"

DispatchQueue::DispatchQueue(void):
  m_dispatchCap(1024),
  m_aborted(false)
{}

DispatchQueue::~DispatchQueue(void) {
  // Wipe out each entry in the queue, we can't call any of them because we're in teardown
  for(DispatchThunkBase* thunk : m_dispatchQueue)
    delete thunk;
  
  while (!m_delayedQueue.empty()) {
    DispatchThunkDelayed thunk = m_delayedQueue.top();
    thunk.Reset();
    m_delayedQueue.pop();
  }
}

void DispatchQueue::Abort(void) {
  std::lock_guard<std::mutex> lk(m_dispatchLock);
  m_aborted = true;

  // Do not permit any more lambdas to be pended to our queue:
  m_dispatchCap = 0;

  // Destroy the whole dispatch queue:
  while(!m_dispatchQueue.empty()) {
    delete m_dispatchQueue.front();
    m_dispatchQueue.pop_front();
  }

  // Wake up anyone who is still waiting:
  m_queueUpdated.notify_all();
}

std::chrono::steady_clock::time_point
DispatchQueue::SuggestSoonestWakeupTimeUnsafe(std::chrono::steady_clock::time_point latestTime) const {
  return
    m_delayedQueue.empty() ?

    // Nothing in the queue, no way to suggest a shorter time
    latestTime :

    // Return the shorter of the maximum wait time and the time of the queue ready--we don't want to tell the
    // caller to wait longer than the limit of their interest.
    std::min(
      m_delayedQueue.top().GetReadyTime(),
      latestTime
    );
}

void DispatchQueue::PromoteReadyEventsUnsafe(void) {
  // Move all ready elements out of the delayed queue and into the dispatch queue:
  for(
    auto now = std::chrono::steady_clock::now();
    !m_delayedQueue.empty() && m_delayedQueue.top().GetReadyTime() < now;
    m_delayedQueue.pop()
  )
    // This item's ready time has elapsed, we can add it to our dispatch queue now:
    m_dispatchQueue.push_back(m_delayedQueue.top().Get());
}

void DispatchQueue::DispatchEventUnsafe(std::unique_lock<std::mutex>& lk) {
  // Pull the ready thunk off of the front of the queue and pop it while we hold the lock.
  // Then, we will excecute the call while the lock has been released so we do not create
  // deadlocks.
  std::unique_ptr<DispatchThunkBase> thunk(m_dispatchQueue.front());
  m_dispatchQueue.pop_front();
  bool wasEmpty = m_dispatchQueue.empty();
  lk.unlock();

  MakeAtExit(
    [this, wasEmpty] {
      // If we emptied the queue, we'd like to reobtain the lock and tell everyone
      // that the queue is now empty.
      if(wasEmpty)
        m_queueUpdated.notify_all();
    }
  ),
  (*thunk)();
}

bool DispatchQueue::DispatchEvent(void) {
  std::unique_lock<std::mutex> lk(m_dispatchLock);
  if(m_dispatchQueue.empty())
    return false;

  DispatchEventUnsafe(lk);
  return true;
}

