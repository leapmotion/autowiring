// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfigBase.h"
#include "AutoConfigParser.hpp"
#include "AutoConfigListing.h"
#include "CoreContext.h"

AutoConfigVarBase::AutoConfigVarBase(const std::type_info& ti, bool configured) :
  ContextMember(),
  m_key(autowiring::ExtractKey(ti)),
  m_isConfigured(configured)
{
  m_name = m_key.c_str();
}

void AutoConfigVarBase::AutoInit() {
  auto ctxt = m_context.lock();
  auto listing = ctxt->Inject<AutoConfigListing>();
  auto self = GetSelf<AutoConfigVarBase>();

  listing->NotifyConfigAdded(self);
  if (!IsInherited())
    listing->NotifySetLocally(self);
}

void AutoConfigVarBase::OnSetLocally() {
  auto ctxt = m_context.lock();
  auto listing = ctxt->Inject<AutoConfigListing>(); //get or set
  listing->NotifySetLocally(GetSelf<AutoConfigVarBase>());
}