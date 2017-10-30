// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoCurrentPacketPusher.h"
#include "AutoPacketInternal.hpp"
#include "AutoPacketFactory.h"
#include "SatCounter.h"
#include <algorithm>

using namespace autowiring;

AutoPacketInternal::AutoPacketInternal(AutoPacketFactory& factory, std::shared_ptr<void>&& outstanding) :
  AutoPacket(factory, std::move(outstanding))
{}

AutoPacketInternal::~AutoPacketInternal(void) {}

void AutoPacketInternal::Initialize(bool isFirstPacket) {
  // Mark init time of packet
  this->m_initTime = std::chrono::high_resolution_clock::now();

  // Traverse all descendant contexts, adding their packet subscriber vectors one at a time:
  m_firstCounter = m_parentFactory->CreateSatCounterList();

  // Find all subscribers with no required or optional arguments:
  std::vector<SatCounter*> callCounters;

  {
    std::lock_guard<std::mutex> lk(m_lock);
    for (auto* satCounter = m_firstCounter; satCounter; satCounter = satCounter->flink) {
      // Prime the satisfaction graph for element:
      AddSatCounterUnsafe(*satCounter);
      if (!satCounter->remaining)
        callCounters.push_back(satCounter);
    }
  }

  // Mark timeshifted decorations as unsatisfiable on the first packet
  if (isFirstPacket)
    for (auto& dec : m_decoration_map) {
      auto& key = dec.first;
      if (key.tshift) {
        MarkUnsatisfiable(key);
        MarkSuccessorsUnsatisfiable(key);
      }
    }

  // Call all subscribers with no required or optional arguments:
  // NOTE: This may result in decorations that cause other subscribers to be called.
  {
    autowiring::AutoCurrentPacketPusher pkt(*this);
    for (SatCounter* call : callCounters)
      call->GetCall()(call->GetAutoFilter().ptr(), *this);
  }
}

std::shared_ptr<AutoPacketInternal> AutoPacketInternal::SuccessorInternal(void) {
  return std::static_pointer_cast<AutoPacketInternal>(Successor());
}
