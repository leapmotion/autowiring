// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "CoreContextStateBlock.h"
#include "CoreContext.h"

using namespace autowiring;

RunCounter::RunCounter(const std::shared_ptr<CoreContextStateBlock>& stateBlock, const std::shared_ptr<CoreContext>& owner) :
  stateBlock(stateBlock),

  // Increment the parent's outstanding count as well.  This will be held by the lambda, and will cause the enclosing
  // context's outstanding thread count to be incremented by one as long as we have any threads still running in our
  // context.  This property is relied upon in order to get the Wait function to operate properly.
  parentCount(
    stateBlock->parent ?
    stateBlock->parent->IncrementOutstandingThreadCount(owner->GetParentContext()) :
    nullptr
  ),
  owner(owner)
{}

RunCounter::~RunCounter(void) {
  // Reset the owner before performing any other type of notification, we don't want to hold a reference
  // to the owner and prevent its destruction before signaling or resetting anything
  owner.reset();

  std::weak_ptr<CoreObject> outstanding;
  {
    std::lock_guard<std::mutex> lk(stateBlock->m_lock);

    // Unfortunately, this destructor callback is made before weak pointers are
    // invalidated, which requires that we manually reset the outstanding count
    // We don't want to free memory while holding the lock, so defer
    outstanding = std::move(stateBlock->m_outstanding);
    stateBlock->m_outstanding.reset();
  }

  // Wake everyone up
  stateBlock->m_stateChanged.notify_all();
}

CoreContextStateBlock::CoreContextStateBlock(std::shared_ptr<CoreContextStateBlock> parent) :
  parent(parent)
{}

CoreContextStateBlock::~CoreContextStateBlock(void) {}

std::shared_ptr<RunCounter> CoreContextStateBlock::IncrementOutstandingThreadCount(std::shared_ptr<CoreContext> owner) {
  std::lock_guard<std::mutex> lk(m_lock);
  auto retVal = m_outstanding.lock();
  if (!retVal) {
    retVal = std::make_shared<RunCounter>(shared_from_this(), owner);
    m_outstanding = retVal;
  }
  return retVal;
}
