// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "AutoInjectable.h"

AutoInjectableExpressionBase::AutoInjectableExpressionBase(void){}
AutoInjectableExpressionBase::~AutoInjectableExpressionBase(void){}

AutoInjectable::AutoInjectable(AutoInjectableExpressionBase* pValue) :
  pValue(pValue),
  pFLink(nullptr)
{}

AutoInjectable::AutoInjectable(AutoInjectable&& rhs):
  pValue(rhs.pValue),
  pFLink(rhs.pFLink)
{
  rhs.pFLink = nullptr;
}

AutoInjectable::AutoInjectable(const AutoInjectable &rhs) :
  pValue(rhs.pValue),
  pFLink(rhs.pFLink ? new AutoInjectable(*rhs.pFLink) : nullptr)
{}

AutoInjectable::~AutoInjectable() {
  delete pFLink;
  pFLink = nullptr;
}

template class std::shared_ptr<AutoInjectableExpressionBase>;
