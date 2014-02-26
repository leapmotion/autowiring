// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "CoreThread.h"
#include "CoreContext.h"
#include <boost/thread.hpp>

CoreThread::CoreThread(const char* pName):
  ContextMember(pName),
  m_priority(ThreadPriority::Default),
  m_state(std::make_shared<State>()),
  m_lock(m_state->m_lock),
  m_canAccept(false),
  m_stateCondition(m_state->m_stateCondition)
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
      CoreContext::DebugPrintCurrentExceptionInformation();
    }

    // Signal shutdown on the enclosing context--cannot wait, if we wait we WILL deadlock
    GetContext()->SignalShutdown(false);
  }

  // Unconditionally shut off dispatch delivery:
  RejectDispatchDelivery();

  try {
    // If we are asked to rundown while we still have elements in our dispatch queue,
    // we must try to process them:
    DispatchAllEvents();
  }
  catch(...) {
    // We failed to run down the dispatch queue gracefully, we now need to abort it
    Abort();
  }

  // Notify everyone that we're completed:
  boost::lock_guard<boost::mutex> lk(m_lock);
  m_stop = true;
  m_completed = true;
  m_running = false;

  // Perform a manual notification of teardown listeners
  NotifyTeardownListeners();

  // No longer running, we MUST release the thread pointer to ensure proper teardown order
  m_thisThread.detach();

  // Take a copy of our state condition shared pointer while we still hold a reference to
  // ourselves.  This is the only member out of our collection of members that we actually
  // need to hold a reference to.
  auto state = m_state;

  // Release our hold on the context.  After this point, we have to be VERY CAREFUL that we
  // don't try to refer to any of our own member variables, because our own object may have
  // already gone out of scope.  [this] is potentially dangling.
  pusher.Pop();

  // Notify other threads that we are done.  At this point, any held references that might
  // still exist are held by entities other than ourselves.
  state->m_stateCondition.notify_all();
}

bool CoreThread::ShouldStop(void) const {
  shared_ptr<CoreContext> context = ContextMember::GetContext();
  return m_stop || !context || context->IsShutdown();
}

void CoreThread::ThreadSleep(long millisecond) {
  boost::this_thread::sleep(boost::posix_time::milliseconds(millisecond));
}

bool CoreThread::DelayUntilCanAccept(void) {
  boost::unique_lock<boost::mutex> lk(m_lock);
  m_stateCondition.wait(lk, [this] {return ShouldStop() || CanAccept(); });
  return !ShouldStop();
}

bool CoreThread::Start(std::shared_ptr<Object> outstanding) {
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
  m_thisThread = boost::thread(
    [this, outstanding] {
      this->DoRun();
    }
  );
  return true;
}

void CoreThread::Run() {
  AcceptDispatchDelivery();

  while(!ShouldStop())
    WaitForEvent();
}
