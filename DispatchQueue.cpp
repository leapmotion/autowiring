#include "stdafx.h"
#include "DispatchQueue.h"

DispatchQueue::DispatchQueue(void):
  m_aborted(false),
  m_dispatchCap(1024)
{
}

DispatchQueue::~DispatchQueue(void) {
  if(!m_dispatchQueue.empty())
    throw std::runtime_error("Dispatch queue was not empty at destructor time");
}

void DispatchQueue::Abort(void) {
  boost::lock_guard<boost::mutex> lk(m_dispatchLock);
  m_aborted = true;

  // Rundown:
  for(; !m_dispatchQueue.empty(); m_dispatchQueue.pop_front())
    delete m_dispatchQueue.front();

  // Wake up anyone who is still waiting:
  m_queueUpdated.notify_all();
}

void DispatchQueue::DispatchEventUnsafe(boost::unique_lock<boost::mutex>& lk) {
  DispatchThunkBase* thunk = m_dispatchQueue.front();
  m_dispatchQueue.pop_front();
  lk.unlock();
  (*thunk)();
  delete thunk;
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

bool DispatchQueue::DispatchEvent(void) {
  boost::unique_lock<boost::mutex> lk(m_dispatchLock);
  if(m_aborted)
    throw dispatch_aborted_exception();
  if(m_dispatchQueue.empty())
    return false;

  DispatchEventUnsafe(lk);
  return true;
}
