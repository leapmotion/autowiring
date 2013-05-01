#include "stdafx.h"
#include "DeferredBase.h"
#include "AutowirableSlot.h"

DeferredBase::DeferredBase(Autowirer* pThis, std::weak_ptr<AutowirableSlot> tracker):
  pThis(pThis),
  tracker(tracker)
{
}

DeferredBase::~DeferredBase(void) {
  std::shared_ptr<AutowirableSlot> temp = tracker.lock();
  if(!temp || !temp->IsAutowired())
    // Destruction is occurring before autowiring succeeded, short-circuit
    return;

  // Inform all listeners
  for(size_t i = this->m_postBind.size(); i--; )
    this->m_postBind[i]();
}
