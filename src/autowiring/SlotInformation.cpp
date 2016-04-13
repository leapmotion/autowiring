// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "SlotInformation.h"
#include "AutowirableSlot.h"
#include "InterlockedExchange.h"
#include "thread_specific_ptr.h"
#include MEMORY_HEADER

using namespace autowiring;

// Special file-level allocation with a no-op dtor, because all stack locations are stack-allocated
static autowiring::thread_specific_ptr<SlotInformationStackLocation> tss([](SlotInformationStackLocation*) {});

SlotInformationStackLocation::SlotInformationStackLocation(SlotInformationStumpBase& stump, const void* pObj, size_t extent) :
  stump(stump),
  pObj(pObj),
  extent(extent),
  prior(*tss)
{
  tss.reset(this);
}

template<class T>
void UpdateOrCascadeDelete(T* ptr, const T*& dest) {
  if(!compare_exchange<T>(&dest, ptr, nullptr))
    // Exchange passed, the destination now owns this pointer
    return;

  // Failed the exchange, return here
  std::unique_ptr<const T> prior;
  for(const auto* cur = ptr; cur; cur = cur->pFlink)
    prior.reset(cur);
}

SlotInformationStackLocation::~SlotInformationStackLocation(void) {
  // Replace the prior stack location, we were pushed
  tss.reset(&prior);

  UpdateOrCascadeDelete(m_pCur, stump.pHead);

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
