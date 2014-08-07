// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "SlotInformation.h"
#include "InterlockedExchange.h"
#include "Autowired.h"
#include "thread_specific_ptr.h"
#include MEMORY_HEADER

// Special file-level allocation with a no-op dtor, because all stack locations are stack-allocated
static autowiring::thread_specific_ptr<SlotInformationStackLocation> tss([](SlotInformationStackLocation*) {});

SlotInformationStackLocation::SlotInformationStackLocation(SlotInformationStumpBase* pStump, const void* pObj, size_t extent) :
  m_pPrior(tss.get()),
  m_pStump(pStump),
  m_pCur(nullptr),
  m_pObj(pObj),
  m_extent(extent)
{
  tss.reset(this);
}

SlotInformationStackLocation::~SlotInformationStackLocation(void) {
  if(!m_pStump)
    // Rvalue moved, end here
    return;

  // Replace the prior stack location, we were pushed
  tss.reset(m_pPrior);

  if(
    // Do not attempt the update if this stump is initialized already, because it
    // means that someone else has already filled this in.
    !m_pStump->bInitialized &&

    // We can only end here if the swap succeeds, otherwise we're going to have
    // to leave the value that's here already and tear down our list
    !compare_exchange<SlotInformation>(&m_pStump->pHead, m_pCur, nullptr)
  ) {
    // No reason to CAS this boolean field--we're going to unconditionally make
    // this spot initialized.
    m_pStump->bInitialized = true;
    return;
  }

  // Stump already filled in by someone else before we could get to it.  We don't want
  // to leak memory, so we need to destroy our linked list of slot informations.
  std::unique_ptr<const SlotInformation> prior;
  for(const auto* cur = m_pCur; cur; cur = cur->pFlink)
    prior.reset(cur);
}

SlotInformationStackLocation* SlotInformationStackLocation::CurrentStackLocation(void) {
  return tss.get();
}

SlotInformationStumpBase* SlotInformationStackLocation::CurrentStump(void) {
  // Trivial null defaulting:
  return tss.get() ? tss->m_pStump : nullptr;
}

void SlotInformationStackLocation::RegisterSlot(DeferrableAutowiring* pDeferrable) {
  if(!tss.get())
    // Nothing to do, this slot entry is missing
    return;

  if(tss->m_pStump->bInitialized)
    // No reason to continue, stump already initialized
    return;

  if(!tss->Encloses(pDeferrable))
    // Slot is extraneous, falling outside of the bounds of the original object--ignore
    return;

  tss->m_pCur = new SlotInformation(
    tss->m_pCur,
    pDeferrable->GetType(),
    reinterpret_cast<const unsigned char*>(pDeferrable) -
    reinterpret_cast<const unsigned char*>(tss->m_pObj),
    false
  );
}
