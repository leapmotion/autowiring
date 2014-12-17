// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoPacketFactory.h"
#include "AutoPacketInternal.hpp"
#include "fast_pointer_cast.h"
#include "thread_specific_ptr.h"
#include <cmath>

AutoPacketFactory::AutoPacketFactory(void):
  ContextMember("AutoPacketFactory"),
  m_parent(GetContext()->GetParentContext()),
  m_wasStopped(false),
  m_outstanding(0),
  m_packetCount(0),
  m_packetDurationSum(0),
  m_packetDurationSqSum(0)
{}

AutoPacketFactory::~AutoPacketFactory() {
  // Invalidate the pool and recursively invalidate all parents
  Invalidate();
}

std::shared_ptr<AutoPacket> AutoPacketFactory::NewPacket(void) {
  if(ShouldStop())
    throw autowiring_error("Attempted to create a packet on an AutoPacketFactory that was already terminated");
  if(!IsRunning())
    throw autowiring_error("Cannot create a packet until the AutoPacketFactory is started");
  
  // Hack, this currently can't be called while holding a lock
  std::shared_ptr<AutoPacket> retVal;
  
  // Obtain a packet from previous packet if it still exists
  if (auto prev = m_prevPacket.lock()) {
    retVal = prev->Successor();
  } else {
    retVal = ConstructPacket();
  }
  
  // Store new packet as previous packet
  std::lock_guard<std::mutex> lk(m_lock);
  m_prevPacket = retVal;
  return retVal;
}

std::shared_ptr<AutoPacket> AutoPacketFactory::ConstructPacket(void) {
  // Hack, this currently can't be called while holding a lock
  std::shared_ptr<AutoPacketInternal> retVal = std::make_shared<AutoPacketInternal>(*this, GetInternalOutstanding());
  retVal->Initialize();
  return retVal;
}

bool AutoPacketFactory::IsAutoPacketType(const std::type_info& dataType) {
  return
    dataType == typeid(AutoPacket) ||
    dataType == typeid(auto_arg<AutoPacket&>::id_type) ||
    dataType == typeid(auto_arg<const AutoPacket&>::id_type);
}

bool AutoPacketFactory::Start(std::shared_ptr<Object> outstanding) {
  std::lock_guard<std::mutex> lk(m_lock);
  if(m_wasStopped)
    // Cannot start if already stopped
    return false;
    
  m_outstanding = outstanding;
  m_stateCondition.notify_all();
  return true;
}

std::shared_ptr<void> AutoPacketFactory::GetInternalOutstanding(void) {
  auto retVal = m_outstandingInternal.lock();
  if (retVal)
    return retVal;

  auto outstanding = m_outstanding;
  retVal = std::shared_ptr<void>(
    (void*)1,
    [this, outstanding] (void*) mutable {
      std::lock_guard<std::mutex> lk(m_lock);
      m_stateCondition.notify_all();

      // Weak pointer will prevent our lambda from being destroyed, so we manually reset
      // the outstanding counter in order to force it to be reset here
      outstanding.reset();
    }
  );
  m_outstandingInternal = retVal;
  return retVal;
}

void AutoPacketFactory::Stop(bool graceful) {
  // Return optimization
  if(m_wasStopped)
    return;

  // Kill the object pool
  Invalidate();

  // Queue of local variables to be destroyed when leaving scope
  std::shared_ptr<Object> outstanding;
  t_autoFilterSet autoFilters;

  // Lock destruction precedes local variables
  std::lock_guard<std::mutex> lk(m_lock);

  // Swap outstanding count into a local var, so we can reset outside of a lock
  outstanding.swap(m_outstanding);

  // Same story with the AutoFilters
  autoFilters.swap(m_autoFilters);

  // Now we can lock, update state, and notify any listeners
  m_wasStopped = true;
  m_stateCondition.notify_all();
}

void AutoPacketFactory::Clear(void) {
  // Simple handoff to Stop is sufficient
  Stop(false);
}

void AutoPacketFactory::Wait(void) {
  std::unique_lock<std::mutex> lk(m_lock);
  m_stateCondition.wait(
    lk,
    [this]{
      return ShouldStop() && m_outstandingInternal.expired();
    }
  );
}

void AutoPacketFactory::Invalidate(void) {
  if(m_parent)
    m_parent->Invalidate();
}

void AutoPacketFactory::AddSubscriber(const AutoFilterDescriptor& rhs) {
  (std::lock_guard<std::mutex>)m_lock,
  m_autoFilters.insert(rhs);

  // Trigger object pool reset after releasing the lock.  While it's possible that some
  // packets may be issued between lock reset and object pool reset, these packets will
  // not be specifically invalid; they will simply result in late delivery to certain
  // recipients.  Eventually, all packets will be reset and released.
  Invalidate();
}

void AutoPacketFactory::RemoveSubscriber(const AutoFilterDescriptor& autoFilter) {
  // Trivial removal from the autofilter set:
  {
    std::lock_guard<std::mutex> lk(m_lock);
    m_autoFilters.erase(autoFilter);
  }

  // Regeneration of the packet pool for the same reason as described in AddSubscriber
  Invalidate();
}

AutoFilterDescriptor AutoPacketFactory::GetTypeDescriptorUnsafe(const std::type_info* nodeType) {
  //ASSUME: type_info uniquely specifies descriptor
  for (auto& af : m_autoFilters)
    if (af.GetAutoFilterTypeInfo() == nodeType)
      return af;

  return AutoFilterDescriptor();
}

size_t AutoPacketFactory::GetOutstanding(void) const {
  return m_outstandingInternal.use_count();
}

void AutoPacketFactory::RecordPacketDuration(std::chrono::nanoseconds duration) {
  std::unique_lock<std::mutex> lk(m_lock);
  ++m_packetCount;
  m_packetDurationSum += duration.count();
  m_packetDurationSqSum += duration.count() * duration.count();
}

double AutoPacketFactory::GetMeanPacketLifetime(void) {
  return m_packetDurationSum / m_packetCount;
}

double AutoPacketFactory::GetPacketLifetimeStandardDeviation(void) {
  double mean = m_packetDurationSum / m_packetCount;
  double variance = m_packetDurationSqSum / m_packetCount - mean * mean;
  return std::sqrt(variance >= 0.0 ? variance : 0.0 );
}

void AutoPacketFactory::ResetPacketStatistics(void) {
  std::unique_lock<std::mutex> lk(m_lock);
  m_packetCount = 0;
  m_packetDurationSum = 0.0;
  m_packetDurationSqSum = 0.0;
}

template class RegType<AutoPacketFactory>;
template struct SlotInformationStump<AutoPacketFactory, false>;
template const std::shared_ptr<AutoPacketFactory>& SharedPointerSlot::as<AutoPacketFactory>(void) const;
template std::shared_ptr<AutoPacketFactory> autowiring::fast_pointer_cast<AutoPacketFactory, Object>(const std::shared_ptr<Object>& Other);
