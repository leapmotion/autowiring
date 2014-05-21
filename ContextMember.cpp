#include "stdafx.h"
#include "ContextMember.h"
#include "Autowired.h"
#include "CoreContext.h"

ContextMember::ContextMember(const char* name):
  m_context(CoreContext::CurrentContext()),
  m_name(name),
  m_pSlots(nullptr)
{
}

ContextMember::~ContextMember() {}
