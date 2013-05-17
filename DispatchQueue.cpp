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
  m_dispatchQueue.clear();
  m_queueUpdated.notify_all();
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
  m_dispatchQueue.front()();
  m_dispatchQueue.pop_front();
}

void DispatchQueue::DispatchEvent(void) {
  boost::lock_guard<boost::mutex> lk(m_dispatchLock);
  if(m_aborted)
    throw dispatch_aborted_exception();

  if(!m_dispatchQueue.empty()) {
    m_dispatchQueue.front()();
    m_dispatchQueue.pop_front();
  }
}