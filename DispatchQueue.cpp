// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "DispatchQueue.h"
#include "at_exit.h"

DispatchQueue::DispatchQueue(void):
  m_aborted(false),
  m_dispatchCap(1024)
{
}

DispatchQueue::~DispatchQueue(void) {
  // Teardown:
  for(auto q = m_dispatchQueue.begin(); q != m_dispatchQueue.end(); q++)
    delete *q;
}

void DispatchQueue::Abort(void) {
  boost::lock_guard<boost::mutex> lk(m_dispatchLock);
  m_aborted = true;

  // Rip apart:
  for(auto q = m_dispatchQueue.begin(); q != m_dispatchQueue.end(); q++)
    delete *q;
  m_dispatchQueue.clear();

  // Wake up anyone who is still waiting:
  m_queueUpdated.notify_all();
}

boost::chrono::nanoseconds DispatchQueue::SuggestSleepTimeUnsafe(boost::chrono::nanoseconds maxWaitTime) const {
  if(m_delayedQueue.empty())
    // Nothing in the queue, no way to suggest a shorter time
    return maxWaitTime;

  // If the frontmost time has already elapsed then we can just tell the caller to wake up right now:
  auto now = boost::chrono::high_resolution_clock::now();
  if(now > m_delayedQueue.top().GetReadyTime())
    return boost::chrono::nanoseconds::zero();

  // Return the shorter of the maximum wait time and the time of the queue ready--we don't want to tell the
  // caller to wait longer than the limit of their interest.
  return
    std::min(
      m_delayedQueue.top().GetReadyTime() - boost::chrono::high_resolution_clock::now(),
      maxWaitTime
    );
}

void DispatchQueue::PromoteReadyEventsUnsafe(void) {
  // Move all ready elements out of the delayed queue and into the dispatch queue:
  for(
    auto now = boost::chrono::high_resolution_clock::now();
    !m_delayedQueue.empty() && m_delayedQueue.top().GetReadyTime() < now;
    m_delayedQueue.pop()
  )
    // This item's ready time has elapsed, we can add it to our dispatch queue now:
    m_dispatchQueue.push_back(m_delayedQueue.top().Reset());
}

void DispatchQueue::DispatchEventUnsafe(boost::unique_lock<boost::mutex>& lk) {
  // Pull the ready thunk off of the front of the queue and pop it while we hold the lock.
  // Then, we will excecute the call while the lock has been released so we do not create
  // deadlocks.
  DispatchThunkBase* thunk = m_dispatchQueue.front();
  m_dispatchQueue.pop_front();
  bool wasEmpty = m_dispatchQueue.empty();
  lk.unlock();

  auto generalCleanup = MakeAtExit(
    [this, thunk, wasEmpty] {
      delete thunk;

      // If we emptied the queue, we'd like to reobtain the lock and tell everyone
      // that the queue is now empty.
      if(wasEmpty)
        m_queueUpdated.notify_all();
    }
  );

  (*thunk)();
}

void DispatchQueue::WaitForEvent(void) {
  boost::unique_lock<boost::mutex> lk(m_dispatchLock);
  if(m_aborted)
    throw dispatch_aborted_exception();

  // Unconditional delay:
  m_queueUpdated.wait(lk, [this] () -> bool {
    if(m_aborted)
      throw dispatch_aborted_exception();
    return !this->m_dispatchQueue.empty();
  });
  DispatchEventUnsafe(lk);
}

bool DispatchQueue::WaitForEvent(boost::chrono::milliseconds milliseconds) {
  boost::unique_lock<boost::mutex> lk(m_dispatchLock);
  if(m_aborted)
    throw dispatch_aborted_exception();

  // Recommend a new wait time based on what's in the delayed queue:
  auto waitTime = SuggestSleepTimeUnsafe(milliseconds);

  // Now we wait, either for the timeout to elapse or for the dispatch queue itself to
  // transition to the "aborted" state.
  m_queueUpdated.wait_for(lk, waitTime, [this]() -> bool {
    if(m_aborted)
      throw dispatch_aborted_exception();
    return !this->m_dispatchQueue.empty();
  });

  // We could check the return value of the prior wait_for call, but for clarity, the
  // state of the dispatch queue itself is checked instead.
  if(m_dispatchQueue.empty())
    return false;
  
  DispatchEventUnsafe(lk);
  return true;
}

bool DispatchQueue::WaitForEvent(boost::chrono::steady_clock::time_point wakeTime) {
  if(wakeTime == boost::chrono::steady_clock::time_point::max())
    // Maximal wait--we can optimize by using the zero-arguments version
    return WaitForEvent(), true;

  boost::unique_lock<boost::mutex> lk(m_dispatchLock);
  if(m_aborted)
    throw dispatch_aborted_exception();

  m_queueUpdated.wait_until(lk, wakeTime, [this] () -> bool {
    if(m_aborted)
      throw dispatch_aborted_exception();
    return !this->m_dispatchQueue.empty();
  });
  if(m_dispatchQueue.empty())
    return false;
  
  DispatchEventUnsafe(lk);
  return true;
}

bool DispatchQueue::DispatchEvent(void) {
  boost::unique_lock<boost::mutex> lk(m_dispatchLock);
  if(m_dispatchQueue.empty())
    return false;

  DispatchEventUnsafe(lk);
  return true;
}
