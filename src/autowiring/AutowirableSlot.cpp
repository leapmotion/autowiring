// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutowirableSlot.h"
#include "autowiring_error.h"
#include "CoreContext.h"
#include <cassert>
#include <stdexcept>

using namespace std;

DeferrableAutowiring::DeferrableAutowiring(AnySharedPointer&& witness, const std::shared_ptr<CoreContext>& context) :
  m_ptr(std::move(witness)),
  m_context(context)
{}

DeferrableAutowiring::~DeferrableAutowiring(void) {
  // Clients MUST call CancelAutowiring from their destructor--if this line is being hit,
  // it's because the type being destructed isn't calling CancelAutowiring properly.
  assert(m_context.expired());
}

void DeferrableAutowiring::reset(void) {
  m_ptr.reset();
  CancelAutowiring();
  m_context.reset();
}

void DeferrableAutowiring::CancelAutowiring(void) {
  auto context = m_context.lock();
  if(!context)
    // Nothing to do here, then
    return;

  // Reset our hold on the weak pointer to prevent repeated cancellation:
  m_context.reset();

  // Always finalize this entry:
  auto strategy = GetStrategy();
  if(strategy)
    strategy->Finalize();
}

void DeferrableAutowiring::SatisfyAutowiring(const AnySharedPointer& ptr) {
  m_ptr = ptr;
}
