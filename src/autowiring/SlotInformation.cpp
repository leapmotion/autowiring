// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "SlotInformation.h"
#include "AutowirableSlot.h"
#include "thread_specific_ptr.h"
#include MEMORY_HEADER

using namespace autowiring;

// Special file-level allocation with a no-op dtor, because all stack locations are stack-allocated
static autowiring::thread_specific_ptr<SlotInformationStackLocation> tss([](void*) {});

SlotInformationStackLocation::SlotInformationStackLocation(SlotInformationStumpBase& stump, const void* pObj, size_t extent) :
  stump(stump),
  pObj(pObj),
  extent(extent),
  prior(*tss)
{
  tss.reset(this);
}

SlotInformationStackLocation::~SlotInformationStackLocation(void) {
  // Replace the prior stack location, we were pushed
  tss.reset(&prior);

  const SlotInformation* p = nullptr;
  if (!stump.pHead.compare_exchange_strong(p, m_pCur, std::memory_order_relaxed)) {
    // Failed the exchange, destroy
    std::unique_ptr<SlotInformation> prior;
    for (const auto* cur = m_pCur; cur; cur = cur->pFlink)
      prior.reset(m_pCur);
  }
  ///else
    // Exchange passed, the destination now owns this pointer

  // Unconditionally update to true, no CAS needed
  stump.bInitialized = true;
}

SlotInformationStackLocation* SlotInformationStackLocation::CurrentStackLocation(void) {
  return tss.get();
}

void SlotInformationStackLocation::RegisterSlot(DeferrableAutowiring* pDeferrable) {
  if(!tss.get())
    // Nothing to do, this slot entry is missing
    return;

  if(tss->stump.bInitialized)
    // No reason to continue, stump already initialized
    return;

  if(!tss->Encloses(pDeferrable))
    // Slot is extraneous, falling outside of the bounds of the original object--ignore
    return;

  tss->m_pCur = new SlotInformation(
    tss->m_pCur,
    pDeferrable->GetType(),
    reinterpret_cast<const unsigned char*>(pDeferrable) -
    reinterpret_cast<const unsigned char*>(tss->pObj),
    false
  );
}
