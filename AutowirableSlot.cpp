// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "AutowirableSlot.h"
#include "Autowired.h"
#include "autowiring_error.h"
#include "AutoNetworkMonitor.h"
#include "CoreContext.h"
#include <stdexcept>

using namespace std;

AutowirableSlot::AutowirableSlot(const std::shared_ptr<CoreContext>& context, const std::type_info& type) :
  m_context(context)
{
}

AutowirableSlot::~AutowirableSlot(void) {
  auto context = m_context.lock();
  if(!context)
    return;
 
  // Tell our context we are going away:
  context->UndeferSlot(this);
}

std::shared_ptr<CoreContext> AutowirableSlot::LockContext(void) {
  std::shared_ptr<CoreContext> retVal = m_context.lock();
  if(!retVal)
    throw_rethrowable autowiring_error("Attempted to autowire in a context that is tearing down");
  return retVal;
}
