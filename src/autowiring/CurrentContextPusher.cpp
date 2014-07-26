// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "CurrentContextPusher.h"
#include "GlobalCoreContext.h"

CurrentContextPusher::CurrentContextPusher(void):
  m_prior(CoreContext::CurrentContext())
{}

CurrentContextPusher::CurrentContextPusher(std::shared_ptr<CoreContext> pContext):
  m_prior(pContext->SetCurrent())
{}

CurrentContextPusher::CurrentContextPusher(std::shared_ptr<GlobalCoreContext> pContext) :
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
  std::shared_ptr<CoreContext> retVal;
  if(m_prior) {
    retVal = m_prior->SetCurrent();
    m_prior.reset();
  }
  return retVal;
}
