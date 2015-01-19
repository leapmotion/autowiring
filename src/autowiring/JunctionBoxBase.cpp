// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "JunctionBoxBase.h"
#include "CoreContext.h"

JunctionBoxBase::~JunctionBoxBase(void) {}

void JunctionBoxBase::TerminateAll(const std::list<std::weak_ptr<CoreContext>>& teardown) {
  for (auto teardownWeak : teardown) {
    auto curContext = teardownWeak.lock();
    if (curContext)
      curContext->SignalTerminate(false);
  }
}

void JunctionBoxBase::FilterFiringException(const std::shared_ptr<Object>& pRecipient) const {
  // Obtain the current context and pass control:
  CoreContext::CurrentContext()->FilterFiringException(this, pRecipient.get());
}

std::weak_ptr<CoreContext> JunctionBoxBase::ContextDumbToWeak(CoreContext* pContext) {
  return pContext->shared_from_this();
}
