#include "stdafx.h"
#include "ContextMember.h"
#include "Autowired.h"
#include "CoreContext.h"
#include "SlotInformation.h"

ContextMember::ContextMember(const char* name):
  m_context(CoreContext::CurrentContext()),
  m_name(name),
  m_pSlots(SlotInformationStackLocation::CurrentStackLocation())
{
}

ContextMember::~ContextMember() {}

const SlotInformation* ContextMember::GetSlotInformation(void) const {
  return m_pSlots ? m_pSlots->pHead : nullptr;
}