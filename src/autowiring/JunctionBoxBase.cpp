// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "JunctionBoxBase.h"
#include "CoreContext.h"

JunctionBoxBase::~JunctionBoxBase(void) {}

void JunctionBoxBase::TerminateAll(const std::vector<std::weak_ptr<CoreContext>>& teardown) {
  for(size_t i = teardown.size(); i--;) {
    auto curContext = teardown[i].lock();
    if(curContext)
      curContext->SignalTerminate(false);
  }
}

void JunctionBoxBase::FilterFiringException(const std::shared_ptr<EventReceiver>& pRecipient) const {
  // Obtain the current context and pass control:
  CoreContext::CurrentContext()->FilterFiringException(this, pRecipient.get());
}

std::weak_ptr<CoreContext> JunctionBoxBase::ContextDumbToWeak(CoreContext* pContext) {
  return pContext->shared_from_this();
}
