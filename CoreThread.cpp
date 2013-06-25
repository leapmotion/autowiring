// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
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
  m_completed(false),
  m_canAccept(false)
{
}

void CoreThread::DoRun(void) {
  ASSERT(m_running);

  // Make our own session current before we do anything else:
  CurrentContextPusher pusher(GetContext());

  // Set the thread name no matter what:
  if(GetName())
    SetCurrentThreadName();

  // Now we wait for the thread to be good to go:
  DelayUntilReady();
  try {
    Run();
  } catch(dispatch_aborted_exception&) {
    // Okay, this is fine, a dispatcher is terminating--this is a normal way to
    // end a dispatcher loop.
  } catch(...) {
    try {
      // Ask that the enclosing context filter this exception, if possible:
      GetContext()->FilterException();
    } catch(...) {
      // Generic exception, unhandled, we can't print anything off.
      Autowirer::DebugPrintCurrentExceptionInformation();
    }

    // Signal shutdown on the enclosing context
    GetContext()->SignalShutdown();
  }

  // Unconditionally shut off dispatch delivery:
  RejectDispatchDelivery();

  // Notify everyone that we're completed:
  boost::lock_guard<boost::mutex> lk(m_lock);
  m_completed = true;
  m_running = false;
  m_stateCondition.notify_all();
}

bool CoreThread::ShouldStop(void) const {
  shared_ptr<CoreContext> context = ContextMember::GetContext();
  return m_stop || !context || context->ShouldStop();
}

void CoreThread::ThreadSleep(long millisecond) {
  boost::this_thread::sleep(boost::posix_time::milliseconds(millisecond));
}

bool CoreThread::DelayUntilCanAccept(void) {
  boost::unique_lock<boost::mutex> lk(m_lock);
  m_stateCondition.wait(lk, [this] () {return ShouldStop() || CanAccept();});
  return ShouldStop();
}

bool CoreThread::Start(void) {
  std::shared_ptr<CoreContext> context = m_context.lock();
  if(!context)
    return false;

  {
    boost::lock_guard<boost::mutex> lk(m_lock);
    if(m_running)
      // Already running, short-circuit
      return true;

    // Currently running:
    m_running = true;
    m_stateCondition.notify_all();
  }

  // Kick off a thread and return here
  boost::thread(ThreadStatusMaintainer(this, context));
  return true;
}

void CoreThread::Run() {
  AcceptDispatchDelivery();
  while(!ShouldStop())
    WaitForEvent();
}
