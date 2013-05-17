#include "stdafx.h"
#include "DispatchQueue.h"

DispatchQueue::DispatchQueue(void):
  m_aborted(false)
{
}

DispatchQueue::~DispatchQueue(void) {
  if(!m_dispatchQueue.empty())
    throw std::runtime_error("Dispatch queue was not empty at destructor time");
}

void DispatchQueue::Abort(void) {
  boost::lock_guard<boost::mutex> lk(m_dispatchLock);
  m_aborted = true;

  // Clear everything
  m_dispatchQueue.clear();

  // Wake up anyone who is still waiting:
  m_queueUpdated.notify_all();
}

void DispatchQueue::DispatchEventUnsafe(boost::unique_lock<boost::mutex>& lk) {
  std::unique_ptr<t_thunk> thunk = std::move(m_dispatchQueue.front());
  m_dispatchQueue.pop_front();
  lk.unlock();
  (*thunk)();
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

void DispatchQueue::DispatchEvent(void) {
  boost::unique_lock<boost::mutex> lk(m_dispatchLock);
  if(m_aborted)
    throw dispatch_aborted_exception();
  DispatchEventUnsafe(lk);
}