#include "stdafx.h"
#include "ContextMember.h"
#include "Autowired.h"
#include "CoreContext.h"
#include "SlotInformation.h"

ContextMember::ContextMember(const char* name):
  m_context(CoreContext::CurrentContext()),
  m_name(name)
{
}

ContextMember::~ContextMember() {}
