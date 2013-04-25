#include "stdafx.h"
#include "AutowirableSlot.h"
#include "CoreContext.h"
#include <stdexcept>

using namespace std;

AutowirableSlot::AutowirableSlot(void):
  m_context(GetCurrentContext())
{
  m_tracker = cpp11::shared_ptr<AutowirableSlot>(this, NullOp<AutowirableSlot*>);
}

void AutowirableSlot::NotifyWhenAutowired(const cpp11::function<void()>& listener) {
  cpp11::shared_ptr<CoreContext> context = LockContext();

  // Punt control over to the context
  context->NotifyWhenAutowired(*this, listener);
}

cpp11::shared_ptr<CoreContext> AutowirableSlot::LockContext(void) {
  cpp11::shared_ptr<CoreContext> retVal = m_context.lock();
  if(!retVal)
    throw std::runtime_error("Attempted to autowire in a context that is tearing down");
  return retVal;
}
