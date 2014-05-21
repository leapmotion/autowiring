#include "stdafx.h"
#include "SlotInformation.h"
#include "InterlockedExchange.h"
#include MEMORY_HEADER
#include <boost/thread/tss.hpp>

// Special file-level allocation with a no-op dtor, because all stack locations are stack-allocated
static boost::thread_specific_ptr<SlotInformationStackLocation> tss([](SlotInformationStackLocation*) {});

SlotInformationStump::~SlotInformationStump(void) {}

SlotInformationStackLocation::SlotInformationStackLocation(SlotInformationStackLocation&& rhs):
  m_pPrior(rhs.m_pPrior),
  m_pHead(rhs.m_pHead),
  m_pCur(rhs.m_pCur),
  m_pObj(rhs.m_pObj),
  m_pContextMember(rhs.m_pContextMember)
{}

SlotInformationStackLocation::SlotInformationStackLocation(SlotInformationStump* pStump, const void* pObj, const void* pContextMember) :
  m_pPrior(tss.release()),
  m_pHead(pStump),
  m_pCur(nullptr),
  m_pObj(pObj),
  m_pContextMember(pContextMember)
{
  tss.reset(this);
}

SlotInformationStackLocation::~SlotInformationStackLocation(void) {
  // Always replace the prior stack location
  tss.reset(m_pPrior);

  if(!m_pHead)
    // Nothing to do, no slots were constructed
    return;

  // Update the stump location with our new value:
  if(!compare_exchange<SlotInformation>(&m_pHead->pHead, m_pCur, nullptr))
    // Success, we can return
    return;

  // Stump already filled in, prevent leaks by destroying this stump entry:
  std::unique_ptr<const SlotInformation> prior;
  for(const auto* cur = m_pCur; cur; cur = cur->pFlink)
    prior.reset(cur);
}

SlotInformationStump* SlotInformationStackLocation::CurrentStackLocation(void) {
  // Trivial null defaulting:
  return tss.get() ? tss->m_pHead : nullptr;
}

void SlotInformationStackLocation::RegisterSlot(DeferrableAutowiring* pDeferrable) {
  if(!tss.get())
    // Nothing to do, this slot entry is missing
    return;

  tss->m_pCur = new SlotInformation(
    tss->m_pCur,
    pDeferrable->GetType(),
    reinterpret_cast<const unsigned char*>(pDeferrable) -
    reinterpret_cast<const unsigned char*>(tss->m_pObj),
    false
  );
}
