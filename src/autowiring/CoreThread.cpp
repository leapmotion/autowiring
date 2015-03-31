// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "CoreThread.h"
#include "Autowired.h"
#include "BasicThreadStateBlock.h"

CoreThread::CoreThread(const char* pName):
  BasicThread(pName)
{}

CoreThread::~CoreThread(void){}

void CoreThread::DoRunLoopCleanup(std::shared_ptr<CoreContext>&& ctxt, std::shared_ptr<CoreObject>&& refTracker) {
  try {
    // If we are asked to rundown while we still have elements in our dispatch queue,
    // we must try to process them:
    CurrentContextPusher pshr(ctxt);
    DispatchAllEvents();
  }
  catch(...) {
    // We failed to run down the dispatch queue gracefully, we now need to abort it
    Abort();
  }

  // Handoff to base class:
  BasicThread::DoRunLoopCleanup(std::move(ctxt), std::move(refTracker));
}

void CoreThread::Run() {
  while(!ShouldStop())
    WaitForEvent();
}

void CoreThread::OnStop(bool graceful) {
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
}
