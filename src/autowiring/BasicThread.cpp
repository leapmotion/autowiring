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
#include <symphony/symphony.h>

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

  // Update the thread priority.  This value may have been assigned before we started.  In that case,
  // we want to be sure we get the correct value assigned eventually.
  SetThreadPriority(m_priority);

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

  // Tell our CoreRunnable parent that we're done to ensure that our reference count will be cleared
  Stop(false);

  // Release our hold on the context.  There is still at least one more hold through the refTracker
  ctxt.reset();

  // Detach.  This is just a simple memory free, destruction of the lambda should have no side-effects
  state->m_thisThread.detach();

  // The reference tracker internally holds a reference to the CoreContext.  If this is the last
  // reference tracker and the context is not otherwise referenced, this reset step may potentially
  // cause the context to be destroyed.  In that case, [this] is dangling and further references to
  // [this] must be avoided.
  refTracker.reset();

  // Notify other threads that we are done.  We are using the [state] local variable to do this, thereby
  // avoiding an invalid reference to [this].
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

  // Place the new thread entity directly in the space where it goes.  This
  // enables us to decide in advance the exact location in memory where the
  // object will be stored.
  auto outstanding = GetOutstanding();
  m_state->m_thisThread.~thread();
  new (&m_state->m_thisThread) std::thread([this, outstanding] () mutable {
    auto g = symphony::create_group(__FUNCTION__);
    auto k_with_attrib = symphony::create_cpu_kernel([this, outstanding] () mutable {
       this->DoRun(std::move(outstanding));
     });
    k_with_attrib.set_little();
    symphony::affinity::set(symphony::affinity::settings(symphony::affinity::cores::little,
       true, symphony::affinity::mode::allow_local_setting));
    g->launch(k_with_attrib);
    g->wait_for();
  });

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

std::shared_ptr<void> BasicThread::GetThread(void) const {
  // Return an aliased shared pointer
  return {
    m_state,
    &m_state->m_thisThread
  };
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
