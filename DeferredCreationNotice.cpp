#include "stdafx.h"
#include "DeferredCreationNotice.h"
#include "CoreContext.h"

DeferredCreationNotice::DeferredCreationNotice(const char* name, std::shared_ptr<CoreContext> ctxt):
  m_name(name),
  m_ctxt(ctxt)
{
}

DeferredCreationNotice::~DeferredCreationNotice(void)
{
  // Broadcast:
  std::shared_ptr<CoreContext> parent = std::dynamic_pointer_cast<CoreContext, Autowirer>(m_ctxt->GetParentContext());
  if(!parent)
    // Short-circuit return, teardown must be underway
    return;

  // Parent context resolved, broadcast the notice:
  parent->BroadcastContextCreationNotice(m_name, m_ctxt);
}
