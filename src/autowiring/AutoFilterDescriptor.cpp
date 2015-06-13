// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoFilterDescriptor.h"

AutoFilterDescriptorStub::AutoFilterDescriptorStub(const std::type_info* pType, autowiring::altitude altitude, const AutoFilterArgument* pArgs, bool deferred, t_extractedCall pCall) :
  m_pType(pType),
  m_altitude(altitude),
  m_pArgs(pArgs),
  m_deferred(deferred),
  m_pCall(pCall)
{
  for(auto pArg = m_pArgs; *pArg; pArg++) {
    m_arity++;
      
    // time shifted arguments arn't required
    if (pArg->is_input)
      ++m_requiredCount;
  }
}

const AutoFilterArgument* AutoFilterDescriptorStub::GetArgumentType(const std::type_info* argType) const {
  for (auto pArg = m_pArgs; *pArg; pArg++)
    if (pArg->ti == argType)
      return pArg;
  return nullptr;
}
