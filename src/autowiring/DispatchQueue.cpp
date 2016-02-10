// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "DispatchQueue.h"
#include "at_exit.h"
#include <assert.h>

DispatchQueue::DispatchQueue(void) {}

DispatchQueue::DispatchQueue(size_t dispatchCap):
  m_dispatchCap(dispatchCap)
{}

DispatchQueue::DispatchQueue(DispatchQueue&& q):
  m_dispatchCap(q.m_dispatchCap),
  onAborted(std::move(q.onAborted))
{
  if (!onAborted)
    *this += std::move(q);
}

DispatchQueue::~DispatchQueue(void) {
  // Wipe out each entry in the queue, we can't call any of them because we're in teardown
  for (auto cur = m_pHead; cur;) {
    auto next = cur->m_pFlink;
    delete cur;
    cur = next;
  }
}

bool DispatchQueue::PromoteReadyDispatchersUnsafe(void) {
  // Move all ready elements out of the delayed queue and into the dispatch queue:
  size_t nInitial = m_delayedQueue.size();

  // String together a chain of things that will be made ready:
  for (
    auto now = std::chrono::steady_clock::now();
    !m_delayedQueue.empty() && m_delayedQueue.top().GetReadyTime() < now;
    m_delayedQueue.pop()
  ) {
    // Update tail if head is already set, otherwise update head:
    auto thunk = m_delayedQueue.top().Release();
    if (m_pHead)
      m_pTail->m_pFlink = thunk;
    else
      m_pHead = thunk;
    m_pTail = thunk;
    m_count++;
  }

  // Something was promoted if the dispatch queue size is different
  return nInitial != m_delayedQueue.size();
}

void DispatchQueue::DispatchEventUnsafe(std::unique_lock<std::mutex>& lk) {
  // Pull the ready thunk off of the front of the queue and pop it while we hold the lock.
  // Then, we will excecute the call while the lock has been released so we do not create
  // deadlocks.
  std::unique_ptr<DispatchThunkBase> thunk(m_pHead);
  m_pHead = thunk->m_pFlink;
  lk.unlock();

  MakeAtExit([&] {
    if (!--m_count) {
      // Notify that we have hit zero:
      std::lock_guard<std::mutex>{ *lk.mutex() };
      m_queueUpdated.notify_all();
    }
  }),
  (*thunk)();
}

void DispatchQueue::TryDispatchEventUnsafe(std::unique_lock<std::mutex>& lk) {
  // Pull the ready thunk off of the front of the queue and pop it while we hold the lock.
  // Then, we will excecute the call while the lock has been released so we do not create
  // deadlocks.
  DispatchThunkBase* pThunk = m_pHead;
  m_pHead = pThunk->m_pFlink;
  lk.unlock();

  try { (*pThunk)(); }
  catch (...) {
    // Failed to execute thunk, put it back
    lk.lock();
    pThunk->m_pFlink = m_pHead;
    m_pHead = pThunk;
    throw;
  }

  if (!--m_count) {
    // Notify that we have hit zero:
    std::lock_guard<std::mutex>{ *lk.mutex() };
    m_queueUpdated.notify_all();
  }
  delete pThunk;
}

void DispatchQueue::Abort(void) {
  // Do not permit any more lambdas to be pended to our queue
  DispatchThunkBase* pHead;
  {
    std::lock_guard<std::mutex> lk(m_dispatchLock);
    onAborted();
    m_dispatchCap = 0;
    pHead = m_pHead;
    m_pHead = nullptr;
    m_pTail = nullptr;
  }

  // Destroy the whole dispatch queue.  Do so in an unsynchronized context in order to prevent
  // reentrancy.
  size_t nTraversed = 0;
  for (auto cur = pHead; cur;) {
    auto next = cur->m_pFlink;
    delete cur;
    cur = next;
    nTraversed++;
  }

  // Decrement the count by the number of entries we actually traversed.  Abort may potentially
  // be called from a lambda function, so assigning this value directly to zero would be an error.
  m_count -= nTraversed;

  // Wake up anyone who is still waiting:
  m_queueUpdated.notify_all();
}

void DispatchQueue::WakeAllWaitingThreads(void) {
  m_version++;
  m_queueUpdated.notify_all();
}

void DispatchQueue::WaitForEvent(void) {
  std::unique_lock<std::mutex> lk(m_dispatchLock);
  if (onAborted)
    throw dispatch_aborted_exception("Dispatch queue was aborted prior to waiting for an event");

  // Unconditional delay:
  uint64_t version = m_version;
  m_queueUpdated.wait(
    lk,
    [this, version] {
      if (onAborted)
        throw dispatch_aborted_exception("Dispatch queue was aborted while waiting for an event");

      return
        // We will need to transition out if the delay queue receives any items:
        !this->m_delayedQueue.empty() ||

        // We also transition out if the dispatch queue has any events:
        this->m_pHead ||

        // Or, finally, if the versions don't match
        version != m_version;
    }
  );

  if (m_pHead) {
    // We have an event, we can just hop over to this variant:
    DispatchEventUnsafe(lk);
    return;
  }

  if (!m_delayedQueue.empty())
    // The delay queue has items but the dispatch queue does not, we need to switch
    // to the suggested sleep timeout variant:
    WaitForEventUnsafe(lk, m_delayedQueue.top().GetReadyTime());
}

bool DispatchQueue::WaitForEvent(std::chrono::milliseconds milliseconds) {
  return WaitForEvent(std::chrono::steady_clock::now() + milliseconds);
}

bool DispatchQueue::WaitForEvent(std::chrono::steady_clock::time_point wakeTime) {
  if (wakeTime == std::chrono::steady_clock::time_point::max())
    // Maximal wait--we can optimize by using the zero-arguments version
    return WaitForEvent(), true;

  std::unique_lock<std::mutex> lk(m_dispatchLock);
  return WaitForEventUnsafe(lk, wakeTime);
}

bool DispatchQueue::WaitForEventUnsafe(std::unique_lock<std::mutex>& lk, std::chrono::steady_clock::time_point wakeTime) {
  if (onAborted)
    throw dispatch_aborted_exception("Dispatch queue was aborted prior to waiting for an event");

  while (!m_pHead) {
    // Derive a wakeup time using the high precision timer:
    wakeTime = SuggestSoonestWakeupTimeUnsafe(wakeTime);

    // Now we wait, either for the timeout to elapse or for the dispatch queue itself to
    // transition to the "aborted" state.
    std::cv_status status = m_queueUpdated.wait_until(lk, wakeTime);

    // Short-circuit if the queue was aborted
    if (onAborted)
      throw dispatch_aborted_exception("Dispatch queue was aborted while waiting for an event");

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
  if (!m_pHead && !PromoteReadyDispatchersUnsafe())
    return false;

  DispatchEventUnsafe(lk);
  return true;
}

bool DispatchQueue::TryDispatchEvent(void) {
  std::unique_lock<std::mutex> lk(m_dispatchLock);
  if (!m_pHead && !PromoteReadyDispatchersUnsafe())
    return false;

  TryDispatchEventUnsafe(lk);
  return true;
}

int DispatchQueue::DispatchAllEvents(void) {
  int retVal = 0;
  while(DispatchEvent())
    retVal++;
  return retVal;
}

void DispatchQueue::PendExisting(std::unique_lock<std::mutex>&& lk, DispatchThunkBase* thunk) {
  // Count must be separately maintained:
  m_count++;

  // Linked list setup:
  if (m_pHead)
    m_pTail->m_pFlink = thunk;
  else {
    m_pHead = thunk;
    m_queueUpdated.notify_all();
  }
  m_pTail = thunk;

  // Notification as needed:
  OnPended(std::move(lk));
}

bool DispatchQueue::Barrier(std::chrono::nanoseconds timeout) {
  // Optimistic check first:
  std::unique_lock<std::mutex> lk(m_dispatchLock);

  // Short-circuit if dispatching has been aborted
  if (onAborted)
    throw dispatch_aborted_exception("Dispatch queue was aborted before a timed wait was attempted");

  // Short-circuit if the queue is already empty
  if (!m_count)
    return true;

  // Also short-circuit if zero is specified as the timeout value
  if (timeout.count() == 0)
    return false;

  // Set up the lambda.  Note that the queue size CANNOT be 1, because we just checked to verify
  // that it is non-empty.  Thus, we do not need to signal the m_queueUpdated condition variable.
  auto complete = std::make_shared<bool>(false);
  auto lambda = [complete] { *complete = true; };
  PendExisting(
    std::move(lk),
    new DispatchThunk<decltype(lambda)>(std::move(lambda))
  );
  if (!lk.owns_lock())
    lk.lock();

  // Wait until our variable is satisfied, which might be right away:
  bool rv = m_queueUpdated.wait_for(lk, timeout, [&] { return onAborted || *complete; });
  if (onAborted)
    throw dispatch_aborted_exception("Dispatch queue was aborted during a timed wait");
  return rv;
}

void DispatchQueue::Barrier(void) {
  // Set up the lambda:
  bool complete = false;
  *this += [&] { complete = true; };

  // Obtain the lock, wait until our variable is satisfied, which might be right away:
  std::unique_lock<std::mutex> lk(m_dispatchLock);
  m_queueUpdated.wait(lk, [&] { return onAborted || complete; });
  if (onAborted)
    // At this point, the dispatch queue MUST be completely run down.  We have no outstanding references
    // to our stack-allocated "complete" variable.  Furthermore, after m_aborted is true, no further
    // dispatchers are permitted to be run.
    throw dispatch_aborted_exception("Dispatch queue was aborted while a barrier was invoked");
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

void DispatchQueue::operator+=(DispatchQueue&& rhs) {
  std::unique_lock<std::mutex> lk(m_dispatchLock);

  // Append thunks to our queue
  if (m_pHead)
    m_pTail->m_pFlink = rhs.m_pHead;
  else
    m_pHead = rhs.m_pHead;

  m_pTail = rhs.m_pTail;
  m_count += rhs.m_count;

  // Clear queue from rhs
  rhs.m_pHead = nullptr;
  rhs.m_pTail = nullptr;
  rhs.m_count = 0;

  // Append delayed thunks
  while (!rhs.m_delayedQueue.empty()) {
    const auto& top = rhs.m_delayedQueue.top();
    m_delayedQueue.emplace(top.GetReadyTime(), top.Release());
    rhs.m_delayedQueue.pop();
  }

  // Notification as needed:
  m_queueUpdated.notify_all();
  OnPended(std::move(lk));
}

DispatchQueue::DispatchThunkDelayedExpressionAbs DispatchQueue::operator+=(std::chrono::steady_clock::time_point rhs) {
  return{this, rhs};
}

void DispatchQueue::operator+=(DispatchThunkDelayed&& rhs) {
  bool shouldNotify;
  {
    std::lock_guard<std::mutex> lk(m_dispatchLock);
    m_delayedQueue.push(std::forward<DispatchThunkDelayed>(rhs));
    shouldNotify = m_delayedQueue.top().GetReadyTime() == rhs.GetReadyTime() && !m_count;
  }

  if(shouldNotify)
    // We're becoming the new next-to-execute entity, dispatch queue currently empty, trigger wakeup
    // so our newly pended delay thunk is eventually processed.
    m_queueUpdated.notify_all();
}
