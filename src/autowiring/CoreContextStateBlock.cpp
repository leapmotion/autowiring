// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "CoreContextStateBlock.h"
#include "CoreContext.h"

CoreContextStateBlock::CoreContextStateBlock(std::shared_ptr<CoreContextStateBlock> parent) :
  parent(parent)
{}

CoreContextStateBlock::~CoreContextStateBlock(void) {}

std::shared_ptr<CoreObject> CoreContextStateBlock::IncrementOutstandingThreadCount(std::shared_ptr<CoreContext> owner) {
  // Optimistic check
  std::shared_ptr<CoreObject> retVal = m_outstanding.lock();
  if (retVal)
    return retVal;

  // Double-check
  std::lock_guard<std::mutex> lk(m_lock);
  retVal = m_outstanding.lock();
  if (retVal)
    return retVal;

  // Increment the parent's outstanding count as well.  This will be held by the lambda, and will cause the enclosing
  // context's outstanding thread count to be incremented by one as long as we have any threads still running in our
  // context.  This property is relied upon in order to get the Wait function to operate properly.
  std::shared_ptr<CoreObject> parentCount;
  if (parent)
    parentCount = parent->IncrementOutstandingThreadCount(owner->GetParentContext());

  auto self = shared_from_this();
  retVal.reset(
    (CoreObject*) 1,
    [this, self, parentCount, owner](CoreObject*) mutable {
      // Reset the owner before performing any other type of notification, we don't want to hold a reference
      // to the owner and prevent its destruction before signalling or resetting anything
      owner.reset();

      // Object being destroyed, notify all recipients
      std::lock_guard<std::mutex> lk(m_lock);

      // Unfortunately, this destructor callback is made before weak pointers are
      // invalidated, which requires that we manually reset the outstanding count
      m_outstanding.reset();

      // Wake everyone up
      m_stateChanged.notify_all();
    }
  );

  m_outstanding = retVal;
  return retVal;
}
