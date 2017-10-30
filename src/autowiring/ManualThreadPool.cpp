// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ManualThreadPool.h"
#include "at_exit.h"

using namespace autowiring;

void ThreadPoolToken::Leave(void) {
  cancelled = true;
  manualPool->WakeAllWaitingThreads();
}

ManualThreadPool::ManualThreadPool(void)
{
}

ManualThreadPool::~ManualThreadPool(void)
{
}

void ManualThreadPool::OnStartUnsafe(void)
{}

void ManualThreadPool::OnStop(void)
{}

std::shared_ptr<ThreadPoolToken> ManualThreadPool::PrepareJoin(void) {
  return std::make_shared<ThreadPoolToken>(
    std::static_pointer_cast<ManualThreadPool>(shared_from_this())
  );
}

void ManualThreadPool::Join(std::shared_ptr<ThreadPoolToken> token) {
  m_outstanding++;
  auto clear = MakeAtExit([&] { m_outstanding--; });
  try {
    while (!token->cancelled)
      WaitForEvent();
  }
  catch (dispatch_aborted_exception&) {
    // Dispatch aborted exception, back out
  }
  catch (...) {
    // Unknown, consume exception and end here
  }
}

bool ManualThreadPool::Submit(std::unique_ptr<DispatchThunkBase>&& thunk) {
  // Add some more work
  AddExisting(std::move(thunk));
  return true;
}
