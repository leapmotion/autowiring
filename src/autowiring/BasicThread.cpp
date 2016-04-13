// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "BasicThread.h"
#include "autowiring_error.h"
#include "BasicThreadStateBlock.h"
#include "ContextEnumerator.h"
#include "CoreContext.h"
#include "CurrentContextPusher.h"
#include "dispatch_aborted_exception.h"
#include "GlobalCoreContext.h"
#include "fast_pointer_cast.h"
#include <cassert>
#include ATOMIC_HEADER

using namespace autowiring;

static auto mainTID = std::this_thread::get_id();

BasicThread::BasicThread(const char* pName):
  ContextMember(pName),
  m_state(std::make_shared<BasicThreadStateBlock>())
{}

BasicThread::~BasicThread(void) {
  NotifyTeardownListeners();
}

std::mutex& BasicThread::GetLock(void) const {
  return m_state->m_lock;
}

void BasicThread::DoRun(std::shared_ptr<CoreObject>&& refTracker) {
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

void BasicThread::DoRunLoopCleanup(std::shared_ptr<CoreContext>&& ctxt, std::shared_ptr<CoreObject>&& refTracker) {
  // Take a copy of our state condition shared pointer while we still hold a reference to
  // ourselves.  This is the only member out of our collection of members that we actually
  // need to hold a reference to.
  auto state = m_state;

  // Transition to stopped state.  Synchronization not required, transitions are all one-way
  m_stop = true;
  m_running = false;

  // Perform a manual notification of teardown listeners
  NotifyTeardownListeners();

  // Tell our CoreRunnable parent that we're done to ensure that our reference count will be cleared.
  Stop(false);

  // Release our hold on the context.  After this point, we have to be VERY CAREFUL that we
  // don't try to refer to any of our own member variables, because our own object may have
  // already gone out of scope.  [this] is potentially dangling.
  ctxt.reset();

  // Clear our reference tracker, which will notify anyone who is asleep and also maybe
  // will destroy the entire underlying context.
  refTracker.reset();

  // MUST detach here.  By this point in the application, it's possible that `this` has already been
  // deleted.  If that's the case, `state.unique()` is true, and when we go out of scope, the destructor
  // for m_thisThread will be invoked.  If that happens, the destructor will block for the held thread
  // to quit--and, in this case, the thread which is being held is actually us.  Blocking on it, in that
  // case, would be a trivial deadlock.  So, because we're about to quit anyway, we simply detach the
  // thread and prepare for final teardown operations.
  state->m_thisThread.detach();

  // Notify other threads that we are done.  At this point, any held references that might still exist
  // notification must happen from a synchronized level in order to ensure proper ordering.
  std::lock_guard<std::mutex>{state->m_lock},
  state->m_completed = true;
  state->m_stateCondition.notify_all();
}

void BasicThread::WaitForStateUpdate(const std::function<bool()>& fn) const {
  std::unique_lock<std::mutex> lk(m_state->m_lock);
  m_state->m_stateCondition.wait(
    lk,
    [&fn, this] {
      return fn() || ShouldStop();
    }
  );
  if(ShouldStop())
    throw dispatch_aborted_exception("Thread was stopped before the function returned true");
}

void BasicThread::PerformStatusUpdate(const std::function<void()>& fn) const {
  std::unique_lock<std::mutex>{m_state->m_lock},
  fn();
  m_state->m_stateCondition.notify_all();
}

bool BasicThread::OnStart(void) {
  std::shared_ptr<CoreContext> context = m_context.lock();
  if(!context)
    return false;

  // Currently running and started:
  m_running = true;
  m_wasStarted = true;

  // Place the new thread entity directly in the space where it goes to avoid
  // any kind of races arising from asynchronous access to this space
  auto outstanding = GetOutstanding();
  m_state->m_thisThread.~thread();
  new (&m_state->m_thisThread) std::thread(
    [this, outstanding] () mutable {
      this->DoRun(std::move(outstanding));
    }
  );
  return true;
}

void BasicThread::OnStop(bool graceful) {
  // If we were never started, we need to set our completed flag to true
  if (!m_wasStarted)
    m_state->m_completed = true;

  // Always invoke stop handler:
  OnStop();
}

void BasicThread::DoAdditionalWait(void) {
  // Wait for the run loop cleanup to happen in DoRunLoopCleanup
  std::unique_lock<std::mutex> lk(m_state->m_lock);
  m_state->m_stateCondition.wait(
    lk,
    [this] { return m_state->m_completed; }
  );
}

bool BasicThread::DoAdditionalWait(std::chrono::nanoseconds timeout) {
  // Wait for the run loop cleanup to happen in DoRunLoopCleanup
  std::unique_lock<std::mutex> lk(m_state->m_lock);
  return m_state->m_stateCondition.wait_for(
    lk,
    timeout,
    [this] { return m_state->m_completed; }
  );
}

bool BasicThread::IsCompleted(void) const {
  return m_state->m_completed;
}

void BasicThread::ForceCoreThreadReidentify(void) {
  for(const auto& ctxt : ContextEnumerator(GlobalCoreContext::Get())) {
    for(const auto& thread : ctxt->CopyBasicThreadList())
      thread->SetCurrentThreadName();
  }
}

void ForceCoreThreadReidentify(void) {
  BasicThread::ForceCoreThreadReidentify();
}

bool BasicThread::IsMainThread(void) {
  return mainTID == std::this_thread::get_id();
}
