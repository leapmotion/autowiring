// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "BasicThread.h"
#include "Autowired.h"
#include "BasicThreadStateBlock.h"
#include "ContextEnumerator.h"
#include "fast_pointer_cast.h"
#include "move_only.h"

// Explicit instantiation of supported time point types:
template<> bool BasicThread::WaitUntil(std::chrono::steady_clock::time_point);
template<> bool BasicThread::WaitUntil(std::chrono::system_clock::time_point);

BasicThread::BasicThread(const char* pName):
  ContextMember(pName),
  m_state(std::make_shared<BasicThreadStateBlock>()),
  m_stop(false),
  m_running(false),
  m_completed(false),
  m_priority(ThreadPriority::Default)
{}

std::mutex& BasicThread::GetLock(void) {
  return m_state->m_lock;
}

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
  }
  catch(dispatch_aborted_exception&) {
    // Okay, this is fine, a dispatcher is terminating--this is a normal way to
    // end a dispatcher loop.
  }
  catch(...) {
    try {
      // Ask that the enclosing context filter this exception, if possible:
      GetContext()->FilterException();
    }
    catch(...) {
      // Generic exception, unhandled, we can't do anything about this
    }

    // Signal shutdown on the enclosing context--cannot wait, if we wait we WILL deadlock
    GetContext()->SignalShutdown(false);
  }

  // Run loop is over, time to clean up
  DoRunLoopCleanup(pusher.Pop(), std::move(refTracker));
}

void BasicThread::DoRunLoopCleanup(std::shared_ptr<CoreContext>&& ctxt, std::shared_ptr<Object>&& refTracker) {
  // Take a copy of our state condition shared pointer while we still hold a reference to
  // ourselves.  This is the only member out of our collection of members that we actually
  // need to hold a reference to.
  auto state = m_state;

  // Perform a manual notification of teardown listeners
  NotifyTeardownListeners();

  // Notify everyone that we're completed:
  std::lock_guard<std::mutex> lk(state->m_lock);
  m_stop = true;
  m_completed = true;
  m_running = false;


  // No longer running, we MUST release the thread pointer to ensure proper teardown order
  state->m_thisThread.detach();

  // Release our hold on the context.  After this point, we have to be VERY CAREFUL that we
  // don't try to refer to any of our own member variables, because our own object may have
  // already gone out of scope.  [this] is potentially dangling.
  ctxt.reset();

  // Clear our reference tracker, which will notify anyone who is asleep and also maybe
  // will destroy the entire underlying context.
  refTracker.reset();

  // Notify other threads that we are done.  At this point, any held references that might
  // still exist are held by entities other than ourselves.
  state->m_stateCondition.notify_all();
}

void BasicThread::WaitForStateUpdate(const std::function<bool()>& fn) {
  std::unique_lock<std::mutex> lk(m_state->m_lock);
  m_state->m_stateCondition.wait(
    lk,
    [&fn, this] {
      return fn() || ShouldStop();
    }
  );
  if(ShouldStop())
    throw dispatch_aborted_exception();
}

void BasicThread::PerformStatusUpdate(const std::function<void()>& fn) {
  std::unique_lock<std::mutex> lk(m_state->m_lock);
  fn();
  m_state->m_stateCondition.notify_all();
}

bool BasicThread::ShouldStop(void) const {
  auto context = ContextMember::GetContext();
  return m_stop || !context || context->IsShutdown();
}

bool BasicThread::IsRunning(void) const {
  std::lock_guard<std::mutex> lk(m_state->m_lock);
  return m_running;
}

bool BasicThread::ThreadSleep(std::chrono::nanoseconds timeout) {
  std::unique_lock<std::mutex> lk(m_state->m_lock);
  return m_state->m_stateCondition.wait_for(lk, timeout, [this] { return ShouldStop(); });
}

bool BasicThread::Start(std::shared_ptr<Object> outstanding) {
  std::shared_ptr<CoreContext> context = m_context.lock();
  if(!context)
    return false;

  {
    std::lock_guard<std::mutex> lk(m_state->m_lock);
    if(m_running)
      // Already running, short-circuit
      return true;

    if(m_completed)
      // Already completed (perhaps cancelled), short-circuit
      return false;

    // Currently running:
    m_running = true;
    m_state->m_stateCondition.notify_all();
  }

  // Kick off a thread and return here
  m_state->m_thisThread = std::thread(
    std::bind(&BasicThread::DoRun, this, std::placeholders::_1),
    std::move(outstanding)
  );
  
  return true;
}

void BasicThread::Wait(void) {
  std::unique_lock<std::mutex> lk(m_state->m_lock);
  m_state->m_stateCondition.wait(
    lk,
    [this] {return this->m_completed; }
  );
}

bool BasicThread::WaitFor(std::chrono::nanoseconds duration) {
  std::unique_lock<std::mutex> lk(m_state->m_lock);
  return m_state->m_stateCondition.wait_for(
    lk,
    duration,
    [this] {return this->m_completed; }
  );
}

template<class TimeType>
bool BasicThread::WaitUntil(TimeType timepoint) {
  std::unique_lock<std::mutex> lk(m_state->m_lock);
  return m_state->m_stateCondition.wait_until(
    lk,
    timepoint,
    [this] {return this->m_completed; }
  );
}

void BasicThread::Stop(bool graceful) {
  {
    std::lock_guard<std::mutex> lk(m_state->m_lock);

    // Trivial return check:
    if(m_stop)
      return;

    // If we're not running, mark ourselves complete
    if(!m_running)
      m_completed = true;

    // Now we send the appropriate trigger:
    m_stop = true;
    m_state->m_stateCondition.notify_all();
  }

  // Event notification takes place outside of the context of a lock
  OnStop();
}

void BasicThread::ForceCoreThreadReidentify(void) {
  for(const auto& ctxt : ContextEnumerator(AutoGlobalContext())) {
    auto threadListCpy = ctxt->CopyBasicThreadList();
    for(auto q = threadListCpy.begin(); q != threadListCpy.end(); q++)
      (**q).SetCurrentThreadName();
  }
}

void ForceCoreThreadReidentify(void) {
  BasicThread::ForceCoreThreadReidentify();
}
