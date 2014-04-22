// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "AutowirableSlot.h"
#include "Autowired.h"
#include "autowiring_error.h"
#include "CoreContext.h"
#include <stdexcept>

using namespace std;

DeferrableAutowiring::DeferrableAutowiring(const std::shared_ptr<CoreContext>& context) :
  m_context(context),
  m_pFlink(nullptr)
{
}

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
