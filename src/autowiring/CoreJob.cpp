// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "CoreJob.h"
#include "CoreContext.h"
#include FUTURE_HEADER

using namespace autowiring;

// Arm doesn't have std::future, but does have std::chrono. We need to convert from std::chrono
// to autoboost::chrono when passing arguments to "std::future"(alias to autoboost::future) on arm.
#if __ANDROID__ && !GCC_CHECK(4, 9)
autoboost::chrono::nanoseconds NanosecondsForFutureWait(const std::chrono::nanoseconds& time) {
  return autoboost::chrono::nanoseconds(time.count());
}
#else
std::chrono::nanoseconds NanosecondsForFutureWait(const std::chrono::nanoseconds& time) {
  return time;
}
#endif

CoreJob::CoreJob(const char* name) :
  ContextMember(name)
{}

CoreJob::~CoreJob(void)
{}

void CoreJob::OnPended(std::unique_lock<std::mutex>&& lk){
  if(!m_curEventInTeardown) {
    // Something is already outstanding, it will handle dispatching for us.
    return;
  }

  if(!m_running) {
    // Nothing to do, we aren't running yet--just hold on to this entry until we are
    // ready to initiate it.
    return;
  }

  // Increment outstanding count because we now have an entry out in a thread pool
  auto outstanding = GetOutstanding();

  if(!outstanding) {
    // We're currently signalled to stop, we must empty the queue and then
    // return here--we can't accept dispatch delivery on a stopped queue.
    for (auto cur = m_pHead; cur;) {
      auto next = cur->m_pFlink;
      delete cur;
      cur = next;
    }
  } else {
    // Need to ask the thread pool to handle our events again:
    m_curEventInTeardown = false;

    std::future<void>* future = static_cast<std::future<void>*>(std::atomic_exchange<void*>(&m_curEvent, nullptr));
    if (future) {
      delete future;
    }

    m_curEvent = new std::future<void>(
      std::async(
        std::launch::async,
        [this, outstanding] () mutable {
          this->DispatchAllAndClearCurrent();
          outstanding.reset();
        }
      ));
  }
}

void CoreJob::DispatchAllAndClearCurrent(void) {
  CurrentContextPusher pshr(GetContext());
  for(;;) {
    // Trivially run down the queue as long as we're in the pool:
    this->DispatchAllEvents();

    // Check the size of the queue.  Could be that someone added something
    // between when we finished looping, and when we obtained the lock, and
    // we don't want to exit our pool if that has happened.
    std::lock_guard<std::mutex> lk(m_dispatchLock);
    if(AreAnyDispatchersReady())
      continue;

    // Indicate that we're tearing down and will be done very soon.  This is
    // a signal to consumers that a call to m_curEvent.wait() will be nearly
    // non-blocking.
    m_curEventInTeardown = true;
    break;
  }

  m_queueUpdated.notify_all();
}

bool CoreJob::OnStart(void) {
  std::shared_ptr<CoreContext> context = m_context.lock();
  if(!context) {
    return false;
  }

  m_running = true;

  std::unique_lock<std::mutex> lk(m_dispatchLock);
  if(m_pHead)
    // Simulate a pending event, because we need to set up our async:
    OnPended(std::move(lk));
  return true;
}

void CoreJob::Abort(void) {
  DispatchQueue::Abort();
  m_running = false;
}

void CoreJob::OnStop(bool graceful) {
  if(graceful) {
    // Pend a call which will invoke Abort once the dispatch queue is done:
    DispatchQueue::Pend(
      [this] {this->Abort();}
    );
  } else {
    // Abort the dispatch queue so anyone waiting will wake up
    Abort();
  }
}

void CoreJob::DoAdditionalWait(void) {
  std::future<void>* future = static_cast<std::future<void>*>(std::atomic_exchange<void*>(&m_curEvent, nullptr));

  if (future) {
    future->wait();
    delete future;
  }
}

bool CoreJob::DoAdditionalWait(std::chrono::nanoseconds timeout) {
  std::future<void>* future = static_cast<std::future<void>*>(std::atomic_exchange<void*>(&m_curEvent, nullptr));
  if (!future)
    return true;

  const auto status = future->wait_for(NanosecondsForFutureWait(timeout));
  delete future;
  return status == std::future_status::ready;
}
