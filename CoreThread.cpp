#include "stdafx.h"
#include "CoreThread.h"
#include "CoreContext.h"
#include "ThreadStatusMaintainer.h"
#include <boost/thread.hpp>

bool CoreThread::ShouldStop(void) const {
  shared_ptr<CoreContext> context = ContextMember::GetContext();
  return m_stop || !context || context->ShouldStop();
}

void CoreThread::ThreadSleep(long millisecond) {
  boost::this_thread::sleep(boost::posix_time::milliseconds(millisecond));
}

bool CoreThread::Start(void) {
  std::shared_ptr<CoreContext> context = GetContext();
  if(!context)
    return false;

  boost::lock_guard<boost::mutex> lk(m_stopLock);
  if(m_running)
    // Already running, short-circuit
    return true;

  // Kick off a thread and return here
  boost::thread(ThreadStatusMaintainer(this, context));
  return true;
}