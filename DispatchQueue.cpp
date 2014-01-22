// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "DispatchQueue.h"

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

void DispatchQueue::DispatchEventUnsafe(boost::unique_lock<boost::mutex>& lk) {
  DispatchThunkBase* thunk = m_dispatchQueue.front();
  m_dispatchQueue.pop_front();
  bool wasEmpty = m_dispatchQueue.empty();
  lk.unlock();
  (*thunk)();
  delete thunk;

  // If we emptied the queue, we'd like to reobtain the lock and tell everyone
  // that the queue is now empty.
  if(wasEmpty) {
    lk.lock();
    m_queueUpdated.notify_all();
  }
}

void DispatchQueue::WaitForEvent(void) {
  boost::unique_lock<boost::mutex> lk(m_dispatchLock);
  if(m_aborted)
    throw dispatch_aborted_exception();

  m_queueUpdated.wait(lk, [this] () -> bool {
    if(m_aborted)
      throw dispatch_aborted_exception();
    return !this->m_dispatchQueue.empty();
  });
  DispatchEventUnsafe(lk);
}

bool DispatchQueue::WaitForEvent(boost::chrono::duration<double, boost::milli> milliseconds) {
  boost::unique_lock<boost::mutex> lk(m_dispatchLock);
  if(m_aborted)
    throw dispatch_aborted_exception();

  m_queueUpdated.wait_for(lk, milliseconds, [this] () -> bool {
    if(m_aborted)
      throw dispatch_aborted_exception();
    return !this->m_dispatchQueue.empty();
  });
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
  if(m_aborted)
    throw dispatch_aborted_exception();
  if(m_dispatchQueue.empty())
    return false;

  DispatchEventUnsafe(lk);
  return true;
}
