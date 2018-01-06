// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "CoreThread.h"
#include "BasicThreadStateBlock.h"
#include "CurrentContextPusher.h"

CoreThread::CoreThread(const char* pName):
  BasicThread(pName)
{}

CoreThread::~CoreThread(void){}

void CoreThread::DoRunLoopCleanup(std::shared_ptr<CoreContext>&& ctxt, std::shared_ptr<CoreObject>&& refTracker) {
  // Kill everything in the dispatch queue and also run it down
  {
    CurrentContextPusher pshr(ctxt);
    // Only allow one thread at a time to clean up the dispatch queue
    std::unique_lock<std::mutex> lk(m_stoppingLock);
    if (ShouldStop()) {
      // If we are stopping, wait until this thread's OnStop function has completed
      m_stoppingCond.wait(lk, [this] { return m_onStopCompleted; });
    }
    Rundown();
  }

  // Handoff to base class:
  BasicThread::DoRunLoopCleanup(std::move(ctxt), std::move(refTracker));
}

void CoreThread::Run() {
  while(!ShouldStop())
    WaitForEvent();
}

void CoreThread::OnStop(bool graceful) {
  // Only allow one thread at a time to clean up the dispatch queue

  // Base class handling first:
  BasicThread::OnStop(graceful);

  if(graceful) {
    // Pend a call which will invoke Abort once the dispatch queue is done:
    DispatchQueue::Pend([this] {
      this->Abort();

      // Notify callers of our new state:
      this->m_state->m_stateCondition.notify_all();
    });
  } else
    // Abort the dispatch queue so anyone waiting will wake up
    DispatchQueue::Abort();

  // When OnStop has completed, then we may continue with DoRunLoopCleanup
  std::lock_guard<std::mutex> lk(m_stoppingLock);
  m_onStopCompleted = true;
  m_stoppingCond.notify_all();
}
