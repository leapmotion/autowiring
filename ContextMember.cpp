// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "ContextMember.h"
#include "Autowired.h"
//#include "AutowiringEvents.h"
#include "CoreContext.h"
#include "AutoNetworkMonitor.h"
#include "Configuration/Config.h"

#ifdef USEAUTONET
#define ENABLE_NET_MON 1
#else
#define ENABLE_NET_MON 0
#endif


ContextMember::ContextMember(const char* name):
  m_name(name),
  m_context(CoreContext::CurrentContext())
{}

ContextMember::~ContextMember() {}

void* ContextMember::operator new(size_t nBytes) {
  void* pRetVal = ::operator new(nBytes);
#if ENABLE_NET_MON
    AutoCurrentContext ctxt;
    shared_ptr<AutoNetworkMonitor> netMon;
    ctxt->FindByType(netMon);
    if(netMon)
      netMon->PreConstruct(pRetVal, nBytes);
#endif
  return pRetVal;
}
