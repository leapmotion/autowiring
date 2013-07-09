// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "CurrentContextPusher.h"
#include "CoreContext.h"

CurrentContextPusher::CurrentContextPusher(void) {
}

CurrentContextPusher::CurrentContextPusher(std::shared_ptr<CoreContext> pContext):
  m_prior(pContext->SetCurrent())
{
}

CurrentContextPusher::CurrentContextPusher(CoreContext* pContext):
  m_prior(pContext->SetCurrent())
{
}

CurrentContextPusher::CurrentContextPusher(CurrentContextPusher&& rhs) {
  std::swap(m_prior, rhs.m_prior);
}

CurrentContextPusher::~CurrentContextPusher(void)
{
  Pop();
}

void CurrentContextPusher::Pop(void) {
  if(m_prior) {
    m_prior->SetCurrent();
    m_prior.reset();
  }
}