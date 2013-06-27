// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "DeferredBase.h"
#include "AutowirableSlot.h"

DeferredBase::DeferredBase(CoreContext* pThis, std::weak_ptr<AutowirableSlot> tracker):
  pThis(pThis),
  tracker(tracker)
{
}

DeferredBase::~DeferredBase(void) {
  std::shared_ptr<AutowirableSlot> temp = tracker.lock();
  if(!temp || !temp->IsAutowired())
    // Destruction is occurring before autowiring succeeded, short-circuit
    return;

  // Inform all listeners
  for(size_t i = this->m_postBind.size(); i--; )
    this->m_postBind[i]();
}
