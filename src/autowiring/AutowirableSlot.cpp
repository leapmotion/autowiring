// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutowirableSlot.h"
#include "autowiring_error.h"
#include "CoreContext.h"
#include <cassert>
#include <stdexcept>

using namespace std;

DeferrableAutowiring::DeferrableAutowiring(AnySharedPointer&& witness, const std::shared_ptr<CoreContext>& context) :
  AnySharedPointer(std::move(witness)),
  m_context(context),
  m_pFlink(nullptr)
{}

DeferrableAutowiring::~DeferrableAutowiring(void) {
  // Clients MUST call CancelAutowiring from their destructor--if this line is being hit,
  // it's because the type being destructed isn't calling CancelAutowiring properly.
  assert(m_context.expired());
}

void DeferrableAutowiring::CancelAutowiring(void) {
  auto context = m_context.lock();
  if(!context)
    // Nothing to do here, then
    return;

  // Reset our hold on the weak pointer to prevent repeated cancellation:
  m_context.reset();

  // Tell our context we are going away:
  context->CancelAutowiringNotification(this);
}
