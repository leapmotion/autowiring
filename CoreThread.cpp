#include "stdafx.h"
#include "CoreThread.h"
#include "CoreContext.h"
#include "ThreadStatusMaintainer.h"
#include <boost/thread.hpp>

CoreThread::CoreThread(const char* pName):
  ContextMember(pName),
  m_stop(false),
  m_ready(false),
  m_running(false),
  m_completed(false)
{
}

bool CoreThread::ShouldStop(void) const {
  shared_ptr<CoreContext> context = ContextMember::GetContext();
  return m_stop || !context || context->ShouldStop();
}

void CoreThread::ThreadSleep(long millisecond) {
  boost::this_thread::sleep(boost::posix_time::milliseconds(millisecond));
}

bool CoreThread::Start(void) {
  std::shared_ptr<CoreContext> context = m_context.lock();
  if(!context)
    return false;

  boost::lock_guard<boost::mutex> lk(m_lock);
  if(m_running)
    // Already running, short-circuit
    return true;

  // Currently running:
  m_running = true;
  m_runCondition.notify_all();

  // Kick off a thread and return here
  boost::thread(ThreadStatusMaintainer(this, context));
  return true;
}