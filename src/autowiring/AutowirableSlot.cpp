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
}

void DeferrableAutowiring::CancelAutowiring(void) {
  auto context = m_context.lock();
  if(!context)
    // Nothing to do here, then
    return;

  m_context.reset();

  UnregisterDeferredAutowire();
}

void DeferrableAutowiring::SatisfyAutowiring(const AnySharedPointer& ptr) {
  m_ptr = ptr;
  UnregisterDeferredAutowire();
}

void DeferrableAutowiring::RegisterDeferredAutowire(autowiring::registration_t&& reg) {
  m_deferred_registration = std::make_unique<autowiring::registration_t>(std::move(reg));
}

void DeferrableAutowiring::UnregisterDeferredAutowire(void) {
  if (m_deferred_registration) {
    *m_deferred_registration->owner -= *m_deferred_registration;
    m_deferred_registration = nullptr;
  }
}
