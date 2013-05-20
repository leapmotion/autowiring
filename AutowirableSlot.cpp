#include "stdafx.h"
#include "AutowirableSlot.h"
#include "Autowired.h"
#include "AutoNetworkMonitor.h"
#include "CoreContext.h"
#include <stdexcept>

using namespace std;

AutowirableSlot::AutowirableSlot(void):
  m_context(GetCurrentContext())
{
  m_tracker = std::shared_ptr<AutowirableSlot>(this, NullOp<AutowirableSlot*>);

  if(ENABLE_NETWORK_MONITOR) {
    // Obtain the network monitor:
    std::shared_ptr<AutoNetworkMonitor> pMon = GetCurrentContext()->FindByType<AutoNetworkMonitor>();
    if(pMon)
      // Pass notification:
      pMon->Notify(*this);
  }
}

void AutowirableSlot::NotifyWhenAutowired(const std::function<void()>& listener) {
  std::shared_ptr<CoreContext> context = LockContext();

  // Punt control over to the context
  context->NotifyWhenAutowired(*this, listener);
}

std::shared_ptr<CoreContext> AutowirableSlot::LockContext(void) {
  std::shared_ptr<CoreContext> retVal = m_context.lock();
  if(!retVal)
    throw std::runtime_error("Attempted to autowire in a context that is tearing down");
  return retVal;
}
