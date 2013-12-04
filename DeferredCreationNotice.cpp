// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "DeferredCreationNotice.h"
#include "CoreContext.h"

DeferredCreationNotice::DeferredCreationNotice(const std::type_info* pSigil, std::shared_ptr<CoreContext> ctxt) :
  m_pSigil(pSigil),
  m_ctxt(ctxt)
{
}

DeferredCreationNotice::~DeferredCreationNotice(void) {
  // Broadcast:
  auto parent = m_ctxt->GetParentContext();
  if(!parent)
    // Short-circuit return, teardown must be underway
    return;

  if(std::uncaught_exception())
    // Another short-circuit, we are being called because we're in an unwind pathway
    return;

  if(m_pSigil && *m_pSigil != typeid(void))
    // Parent context resolved, broadcast the notice:
    parent->BroadcastContextCreationNotice(*m_pSigil, m_ctxt);
}
