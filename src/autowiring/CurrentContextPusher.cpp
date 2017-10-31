// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "CurrentContextPusher.h"
#include "GlobalCoreContext.h"

CurrentContextPusher::CurrentContextPusher(void):
  m_prior(CoreContext::CurrentContextOrNull())
{}

CurrentContextPusher::CurrentContextPusher(CoreContext& context):
  m_prior(context.SetCurrent())
{}

CurrentContextPusher::CurrentContextPusher(const std::shared_ptr<CoreContext>& pContext) :
  m_prior(pContext->SetCurrent())
{}

CurrentContextPusher::CurrentContextPusher(const std::shared_ptr<GlobalCoreContext>& pContext) :
  m_prior(pContext->SetCurrent())
{}

CurrentContextPusher::CurrentContextPusher(CoreContext* pContext):
  m_prior(pContext->SetCurrent())
{}

CurrentContextPusher::CurrentContextPusher(CurrentContextPusher&& rhs) {
  std::swap(m_prior, rhs.m_prior);
}

CurrentContextPusher::~CurrentContextPusher(void) {
  Pop();
}

std::shared_ptr<CoreContext> CurrentContextPusher::Pop(void) {
  if (pop_invoked)
    // Nothing to do
    return nullptr;

  pop_invoked = true;
  auto retVal = CoreContext::SetCurrent(std::move(m_prior));
  m_prior.reset();
  return retVal;
}
