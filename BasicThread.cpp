// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "BasicThread.h"
#include "Autowired.h"
#include "fast_pointer_cast.h"
#include <boost/thread.hpp>

class CoreThread;

BasicThread::BasicThread(const char* pName):
  ContextMember(pName),
  m_priority(ThreadPriority::Default),
  m_state(std::make_shared<State>()),
  m_lock(m_state->m_lock),
  m_stateCondition(m_state->m_stateCondition),
  m_stop(false),
  m_running(false),
  m_completed(false)
{}

void BasicThread::DoRun(std::shared_ptr<Object>&& refTracker) {
  assert(m_running);
  
  // Make our own session current before we do anything else:
  CurrentContextPusher pusher(GetContext());
  
  // Set the thread name no matter what:
  if(GetName())
    SetCurrentThreadName();
  
  // Now we wait for the thread to be good to go:
  try {
    Run();
  } catch(...) {
    try {
      // Ask that the enclosing context filter this exception, if possible:
      GetContext()->FilterException();
    } catch(...) {
      // Generic exception, unhandled, we can't do anything about this
    }
    
    // Signal shutdown on the enclosing context--cannot wait, if we wait we WILL deadlock
    GetContext()->SignalShutdown(false);
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

bool BasicThread::ShouldStop(void) const {
  shared_ptr<CoreContext> context = ContextMember::GetContext();
  return m_stop || !context || context->IsShutdown();
}

void BasicThread::ThreadSleep(long millisecond) {
  boost::this_thread::sleep(boost::posix_time::milliseconds(millisecond));
}


bool BasicThread::Start(std::shared_ptr<Object> outstanding) {
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

void BasicThread::Stop(bool graceful) {
  // Trivial return check:
  if(m_stop)
    return;
  
  // Perform initial stop:
  m_stop = true;
  OnStop();
  m_state->m_stateCondition.notify_all();
}

void BasicThread::ForceCoreThreadReidentify(void) {
  AutoGlobalContext global;
  global->EnumerateChildContexts(
    [](std::shared_ptr<CoreContext> ctxt) {
      auto threadListCpy = ctxt->CopyCoreThreadList();
      for(auto q = threadListCpy.begin(); q != threadListCpy.end(); q++) {
        (**q).SetCurrentThreadName();
      }
    }
  );
}

void ForceCoreThreadReidentify(void) {
  BasicThread::ForceCoreThreadReidentify();
}
