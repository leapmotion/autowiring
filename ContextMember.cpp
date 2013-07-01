// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "ContextMember.h"
#include "Autowired.h"
#include "CoreContext.h"
#include "AutoNetworkMonitor.h"

ContextMember::ContextMember(const char* name):
  m_name(name),
  m_context(CoreContext::CurrentContext())
{
}

ContextMember::~ContextMember() {
}

void* ContextMember::operator new(size_t nBytes) {
  void* pRetVal = ::operator new(nBytes);
  if(ENABLE_NETWORK_MONITOR) {
    AutoCurrentContext ctxt;
    shared_ptr<AutoNetworkMonitor> netMon = ctxt->FindByType<AutoNetworkMonitor>();
    if(netMon)
      netMon->PreConstruct(pRetVal, nBytes);
  }
  return pRetVal;
}
