// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "AutowirableSlot.h"
#include "Autowired.h"
#include "autowiring_error.h"
#include "CoreContext.h"
#include <stdexcept>

using namespace std;

DeferrableAutowiring::DeferrableAutowiring(const std::shared_ptr<CoreContext>& context) :
  m_context(context)
{
}

DeferrableAutowiring::~DeferrableAutowiring(void) {
  auto context = m_context.lock();
  if(context)
    // Tell our context we are going away:
    context->CancelAutowiringNotification(this);
}
