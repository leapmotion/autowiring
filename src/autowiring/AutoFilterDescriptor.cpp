// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoFilterDescriptor.h"

AutoFilterDescriptorStub::AutoFilterDescriptorStub(auto_id type, autowiring::altitude altitude, const AutoFilterArgument* pArgs, bool deferred, autowiring::t_extractedCall pCall) :
  m_type(type),
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

bool AutoFilterDescriptorStub::Provides(const std::type_info& ti) const {
  for (size_t i = GetArity(); i--;)
    if (*m_pArgs[i].id.block->ti == ti)
      return (m_pArgs[i].is_output && !m_pArgs[i].is_rvalue);
  return false;
}

bool AutoFilterDescriptorStub::Consumes(const std::type_info& ti) const {
  for (size_t i = GetArity(); i--;)
    if (*m_pArgs[i].id.block->ti == ti)
      return (m_pArgs[i].is_input && !m_pArgs[i].is_rvalue);
  return false;
}

const AutoFilterArgument* AutoFilterDescriptorStub::GetArgumentType(auto_id argType) const {
  for (auto pArg = m_pArgs; *pArg; pArg++)
    if (pArg->id == argType)
      return pArg;
  return nullptr;
}
