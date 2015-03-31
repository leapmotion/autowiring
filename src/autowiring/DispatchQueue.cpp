// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "DispatchQueue.h"
#include "at_exit.h"
#include <assert.h>

dispatch_aborted_exception::dispatch_aborted_exception(void){}
dispatch_aborted_exception::~dispatch_aborted_exception(void){}

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

bool DispatchQueue::PromoteReadyDispatchersUnsafe(void) {
  // Move all ready elements out of the delayed queue and into the dispatch queue:
  size_t nInitial = m_delayedQueue.size();
  for(
    auto now = std::chrono::steady_clock::now();
    !m_delayedQueue.empty() && m_delayedQueue.top().GetReadyTime() < now;
    m_delayedQueue.pop()
  )
    // This item's ready time has elapsed, we can add it to our dispatch queue now:
    m_dispatchQueue.push_back(m_delayedQueue.top().Get());

  // Something was promoted if the dispatch queue size is different
  return nInitial != m_delayedQueue.size();
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

void DispatchQueue::WaitForEvent(void) {
  std::unique_lock<std::mutex> lk(m_dispatchLock);
  if (m_aborted)
    throw dispatch_aborted_exception();

  // Unconditional delay:
  m_queueUpdated.wait(lk, [this]() -> bool {
    if (m_aborted)
      throw dispatch_aborted_exception();

    return
      // We will need to transition out if the delay queue receives any items:
      !this->m_delayedQueue.empty() ||

      // We also transition out if the dispatch queue has any events:
      !this->m_dispatchQueue.empty();
  });

  if (m_dispatchQueue.empty()) {
    // The delay queue has items but the dispatch queue does not, we need to switch
    // to the suggested sleep timeout variant:
    WaitForEventUnsafe(lk, m_delayedQueue.top().GetReadyTime());
  }
  else {
    // We have an event, we can just hop over to this variant:
    DispatchEventUnsafe(lk);
  }
}

bool DispatchQueue::WaitForEvent(std::chrono::milliseconds milliseconds) {
  return WaitForEvent(std::chrono::steady_clock::now() + milliseconds);
}

bool DispatchQueue::WaitForEvent(std::chrono::steady_clock::time_point wakeTime) {
  if (wakeTime == std::chrono::steady_clock::time_point::max()) {
    // Maximal wait--we can optimize by using the zero-arguments version
    return WaitForEvent(), true;
  }

  std::unique_lock<std::mutex> lk(m_dispatchLock);
  return WaitForEventUnsafe(lk, wakeTime);
}

bool DispatchQueue::WaitForEventUnsafe(std::unique_lock<std::mutex>& lk, std::chrono::steady_clock::time_point wakeTime) {
  if (m_aborted)
    throw dispatch_aborted_exception();

  while (m_dispatchQueue.empty()) {
    // Derive a wakeup time using the high precision timer:
    wakeTime = SuggestSoonestWakeupTimeUnsafe(wakeTime);

    // Now we wait, either for the timeout to elapse or for the dispatch queue itself to
    // transition to the "aborted" state.
    std::cv_status status = m_queueUpdated.wait_until(lk, wakeTime);

    // Short-circuit if the queue was aborted
    if (m_aborted)
      throw dispatch_aborted_exception();

    if (PromoteReadyDispatchersUnsafe())
      // Dispatcher is ready to run!  Exit our loop and dispatch an event
      break;

    if (status == std::cv_status::timeout)
      // Can't proceed, queue is empty and nobody is ready to be run
      return false;
  }

  DispatchEventUnsafe(lk);
  return true;
}

bool DispatchQueue::DispatchEvent(void) {
  std::unique_lock<std::mutex> lk(m_dispatchLock);

  // If the queue is empty and we fail to promote anything, return here
  // Note that, due to short-circuiting, promotion will not take place if the queue is not empty.
  // This behavior is by design.
  if (m_dispatchQueue.empty() && !PromoteReadyDispatchersUnsafe())
    return false;

  assert(!m_dispatchQueue.empty());
  DispatchEventUnsafe(lk);
  return true;
}

int DispatchQueue::DispatchAllEvents(void) {
  int retVal = 0;
  while(DispatchEvent())
    retVal++;
  return retVal;
}

void DispatchQueue::AddExisting(DispatchThunkBase* pBase) {
  std::unique_lock<std::mutex> lk(m_dispatchLock);
  if(m_dispatchQueue.size() >= m_dispatchCap)
    return;
  
  m_dispatchQueue.push_back(pBase);
  m_queueUpdated.notify_all();
  OnPended(std::move(lk));
}

bool DispatchQueue::Barrier(std::chrono::nanoseconds timeout) {
  // Set up the lambda:
  auto complete = std::make_shared<bool>(false);
  *this += [complete] { *complete = true; };

  // Obtain the lock, wait until our variable is satisfied, which might be right away:
  std::unique_lock<std::mutex> lk(m_dispatchLock);
  return m_queueUpdated.wait_for(lk, timeout, [&] {return  *complete; });
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

DispatchQueue::DispatchThunkDelayedExpression DispatchQueue::operator+=(std::chrono::steady_clock::time_point rhs) {
  return DispatchThunkDelayedExpression(this, rhs);
}

void DispatchQueue::operator+=(DispatchThunkDelayed&& rhs) {
  std::lock_guard<std::mutex> lk(m_dispatchLock);
  
  m_delayedQueue.push(std::forward<DispatchThunkDelayed>(rhs));
  if(
    m_delayedQueue.top().GetReadyTime() == rhs.GetReadyTime() &&
    m_dispatchQueue.empty()
  )
    // We're becoming the new next-to-execute entity, dispatch queue currently empty, trigger wakeup
    // so our newly pended delay thunk is eventually processed.
    m_queueUpdated.notify_all();
}
