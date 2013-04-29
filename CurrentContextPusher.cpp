#include "stdafx.h"
#include "CurrentContextPusher.h"
#include "CoreContext.h"

CurrentContextPusher::CurrentContextPusher(std::shared_ptr<CoreContext> pContext):
  m_prior(pContext->SetCurrent())
{
}

CurrentContextPusher::CurrentContextPusher(CoreContext* pContext):
  m_prior(pContext->SetCurrent())
{
}

CurrentContextPusher::~CurrentContextPusher(void)
{
  m_prior->SetCurrent();
}
