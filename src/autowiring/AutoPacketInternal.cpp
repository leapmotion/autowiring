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

void AutoPacketInternal::Initialize(bool isFirstPacket) {
  // Mark init time of packet
  this->m_initTime = std::chrono::high_resolution_clock::now();

  // Traverse all descendant contexts, adding their packet subscriber vectors one at a time:
  m_parentFactory->AppendAutoFiltersTo(m_satCounters);
  
  // Find all subscribers with no required or optional arguments:
  std::vector<SatCounter*> callCounters;
  for (auto& satCounter : m_satCounters) {
    
    // Prime the satisfaction graph for element:
    AddSatCounter(satCounter);
    
    if (satCounter) {
      callCounters.push_back(&satCounter);
    }
  }
  
  // Mark timeshifted decorations as unsatisfiable on the first packet
  if (isFirstPacket) {
    for (auto& dec : m_decorations) {
      auto& key = dec.first;
      if (key.tshift) {
        MarkUnsatisfiable(key);
        MarkSuccessorsUnsatisfiable(key);
      }
    }
  }

  // Call all subscribers with no required or optional arguments:
  // NOTE: This may result in decorations that cause other subscribers to be called.
  for (SatCounter* call : callCounters)
    call->CallAutoFilter(*this);

  // First-call indicated by argumument type AutoPacket&:
  UpdateSatisfaction(DecorationKey(typeid(auto_arg<AutoPacket&>::id_type)));
}

std::shared_ptr<AutoPacketInternal> AutoPacketInternal::SuccessorInternal(void) {
  return std::static_pointer_cast<AutoPacketInternal>(Successor());
}
