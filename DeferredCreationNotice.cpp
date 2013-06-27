// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "DeferredCreationNotice.h"
#include "CoreContext.h"

DeferredCreationNotice::DeferredCreationNotice(const char* name, std::shared_ptr<CoreContext> ctxt):
  m_name(name),
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

  if(m_name)
    // Parent context resolved, broadcast the notice:
    parent->BroadcastContextCreationNotice(m_name, m_ctxt);
}
