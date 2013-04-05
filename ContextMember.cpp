#include "stdafx.h"
#include "ContextMember.h"
#include "CoreContext.h"

ContextMember::ContextMember(const char* name):
  m_name(name),
  m_context(CoreContext::CurrentContext())
{
  ASSERT(name != reinterpret_cast<const char*>(0xCDCDCDCD));

  // A context member MUST NOT be empty.  Even if there was no
  // explicitly assigned context, at the very least, we should
  // be able to recover the global context.
  ASSERT(m_context);
}

ContextMember::~ContextMember() {
}
