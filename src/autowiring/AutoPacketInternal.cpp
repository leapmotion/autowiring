// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoPacketInternal.hpp"
#include "AutoPacketFactory.h"
#include "SatCounter.h"
#include <algorithm>

AutoPacketInternal::AutoPacketInternal(AutoPacketFactory& factory, std::shared_ptr<void>&& outstanding) :
  AutoPacket(factory, std::move(outstanding))
{}

AutoPacketInternal::~AutoPacketInternal(void) {}

void AutoPacketInternal::Initialize(void) {
  // Mark init time of packet
  this->m_initTime = std::chrono::high_resolution_clock::now();

  // Traverse all descendant contexts, adding their packet subscriber vectors one at a time:
  for(const auto& curContext : ContextEnumerator(m_parentFactory->GetContext())) {
    AutowiredFast<AutoPacketFactory> curFactory(curContext);
    if(curFactory)
      // Only insert if this context actually has a packet factory
      curFactory->AppendAutoFiltersTo(m_satCounters);
  }
  
  // Sort, eliminate duplicates
  m_satCounters.sort();
  m_satCounters.erase(std::unique(m_satCounters.begin(), m_satCounters.end()), m_satCounters.end());
  
  // Prime the satisfaction graph for each element:
  for(auto& satCounter : m_satCounters)
    AddSatCounter(satCounter);
  
  // Find all subscribers with no required or optional arguments:
  std::list<SatCounter*> callCounters;
  for (auto& satCounter : m_satCounters)
    if (satCounter)
      callCounters.push_back(&satCounter);

  // Call all subscribers with no required or optional arguments:
  // NOTE: This may result in decorations that cause other subscribers to be called.
  for (SatCounter* call : callCounters)
    call->CallAutoFilter(*this);

  // First-call indicated by argumument type AutoPacket&:
  UpdateSatisfaction(typeid(auto_arg<AutoPacket&>::id_type));
}

std::shared_ptr<AutoPacketInternal> AutoPacketInternal::SuccessorInternal(void) {
  return std::static_pointer_cast<AutoPacketInternal>(Successor());
}
