// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "AutowirableSlot.h"
#include "Autowired.h"
#include "autowiring_error.h"
#include "AutoNetworkMonitor.h"
#include "CoreContext.h"
#include <stdexcept>

using namespace std;

AutowirableSlot::AutowirableSlot(std::weak_ptr<CoreContext> context, const std::type_info& type) :
  m_context(context),
  m_type(type)
{
}

AutowirableSlot::~AutowirableSlot(void) {
  auto context = m_context.lock();
  if(!context)
    return;
 
  // Tell our context we are going away:
  context->UndeferSlot(this);
}

void AutowirableSlot::NotifyWhenAutowired(const std::function<void()>& listener) {
  std::shared_ptr<CoreContext> context = LockContext();

  // Punt control over to the context
  context->NotifyWhenAutowired(*this, listener);
}

std::shared_ptr<CoreContext> AutowirableSlot::LockContext(void) {
  std::shared_ptr<CoreContext> retVal = m_context.lock();
  if(!retVal)
    throw_rethrowable autowiring_error("Attempted to autowire in a context that is tearing down");
  return retVal;
}
