#include "stdafx.h"
#include "ContextCreatorBase.h"
#include "CoreContext.h"

ContextCreatorBase::ContextCreatorBase(void):
  m_context(CoreContext::CurrentContext())
{
}

ContextCreatorBase::~ContextCreatorBase(void) {
}