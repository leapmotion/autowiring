// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoPacketFactory.h"
#include "AutoPacketInternal.hpp"
#include "CoreContext.h"
#include "SatCounter.h"
#include <cmath>

using namespace autowiring;

AutoPacketFactory::AutoPacketFactory(void):
  ContextMember("AutoPacketFactory")
{}

AutoPacketFactory::~AutoPacketFactory() {}

bool AutoPacketFactory::IsRunning(void) const {
  return
    CoreRunnable::IsRunning() ||
    !m_outstandingInternal.expired();
}

std::shared_ptr<AutoPacket> AutoPacketFactory::CurrentPacket(void) {
  std::lock_guard<std::mutex> lk(m_lock);
  return m_curPacket.lock();
}

std::shared_ptr<AutoPacket> AutoPacketFactory::NewPacket(void) {
  std::shared_ptr<AutoPacketInternal> retVal;
  bool isFirstPacket;
  {
    std::lock_guard<std::mutex> lk(m_lock);

    if (ShouldStop())
      throw autowiring_error("Attempted to create a packet on an AutoPacketFactory that was already terminated");
    if (!IsRunning())
      throw autowiring_error("Cannot create a packet until the AutoPacketFactory is started");

    // New packet issued
    isFirstPacket = !m_packetCount;
    ++m_packetCount;

    // Create a new next packet
    retVal = m_nextPacket;
    m_nextPacket = retVal->SuccessorInternal();
    m_curPacket = retVal;
  }

  retVal->Initialize(isFirstPacket);
  return retVal;
}

std::shared_ptr<AutoPacketInternal> AutoPacketFactory::ConstructPacket(void) {
  return std::make_shared<AutoPacketInternal>(*this, GetInternalOutstanding());
}

bool AutoPacketFactory::IsAutoPacketType(const std::type_info& dataType) {
  return
    dataType == typeid(AutoPacket) ||
    dataType == typeid(auto_arg<AutoPacket&>::id_type) ||
    dataType == typeid(auto_arg<const AutoPacket&>::id_type);
}

std::shared_ptr<void> AutoPacketFactory::GetInternalOutstanding(void) {
  auto retVal = m_outstandingInternal.lock();
  if (retVal)
    return retVal;

  std::shared_ptr<CoreObject> outstanding = GetOutstanding();
  retVal = std::shared_ptr<void>(
    (void*)1,
    [this, outstanding] (void*) mutable {
      // Weak pointer will prevent our lambda from being destroyed, so we manually reset
      // the outstanding counter in order to force it to be reset here
      std::lock_guard<std::mutex>{m_lock},
      outstanding.reset();

      // Now we might be ready to wake up, if anyone was waiting on this factory
      m_cv.notify_all();
    }
  );
  m_outstandingInternal = retVal;
  return retVal;
}

std::vector<AutoFilterDescriptor> AutoPacketFactory::GetAutoFilters(void) const {
  std::lock_guard<std::mutex> lk(m_lock);
  std::vector<AutoFilterDescriptor> retVal;
  retVal.assign(m_autoFilters.begin(), m_autoFilters.end());
  return retVal;
}

SatCounter* AutoPacketFactory::CreateSatCounterList(void) const {
  std::lock_guard<std::mutex> lk(m_lock);

  // Trivial return check
  if (m_autoFilters.empty())
    return nullptr;

  auto q = m_autoFilters.begin();

  // Construct the linked list.  This code implements a push-front so that retVal
  // always refers to the first element of the linked list.
  SatCounter* retVal = new SatCounter(*q);
  while (++q != m_autoFilters.end()) {
    SatCounter* next = new SatCounter(*q);
    retVal->blink = next;
    next->flink = retVal;
    retVal = next;
  }
  return retVal;
}

bool AutoPacketFactory::OnStart(void) {
  // Initialize first packet
  std::lock_guard<std::mutex>{m_lock},
  m_nextPacket = ConstructPacket();
  return true;
}

void AutoPacketFactory::OnStop(bool graceful) {
  // Queue of local variables to be destroyed when leaving scope
  t_autoFilterSet autoFilters;
  std::shared_ptr<AutoPacketInternal> nextPacket;

  // Lock destruction precedes local variables
  std::lock_guard<std::mutex>{m_lock},
    autoFilters.swap(m_autoFilters),
    nextPacket.swap(m_nextPacket);
}

void AutoPacketFactory::DoAdditionalWait(void) {
  std::unique_lock<std::mutex> lk(m_lock);
  m_cv.wait(
    lk,
    [this]{
      return ShouldStop() && m_outstandingInternal.expired();
    }
  );
}

bool AutoPacketFactory::DoAdditionalWait(std::chrono::nanoseconds timeout) {
  std::unique_lock<std::mutex> lk(m_lock);
  return m_cv.wait_for(
    lk,
    timeout,
    [this]{
      return ShouldStop() && m_outstandingInternal.expired();
    }
  );
}

void AutoPacketFactory::Clear(void) {
  // Simple handoff to Stop is sufficient
  Stop(false);
}

const AutoFilterDescriptor& AutoPacketFactory::AddSubscriber(const AutoFilterDescriptor& rhs) {
  std::lock_guard<std::mutex> lk(m_lock);
  m_autoFilters.insert(rhs);
  return rhs;
}

void AutoPacketFactory::RemoveSubscriber(const AutoFilterDescriptor& autoFilter) {
  // Trivial removal from the autofilter set:
  std::lock_guard<std::mutex> lk(m_lock);
  m_autoFilters.erase(autoFilter);
}

void AutoPacketFactory::operator-=(const AutoFilterDescriptor& desc) {
  RemoveSubscriber(desc);
}

AutoFilterDescriptor AutoPacketFactory::GetTypeDescriptorUnsafe(auto_id nodeType) {
  //ASSUME: type_info uniquely specifies descriptor
  for (auto& af : m_autoFilters)
    if (af.GetAutoFilterTypeInfo() == nodeType)
      return af;

  return AutoFilterDescriptor();
}

size_t AutoPacketFactory::GetOutstandingPacketCount(void) const {
  // Next packet is stored internally, don't count that packet
  return m_outstandingInternal.use_count() - 1;
}

void AutoPacketFactory::RecordPacketDuration(std::chrono::nanoseconds duration) {
  std::unique_lock<std::mutex> lk(m_lock);
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

template struct autowiring ::SlotInformationStump<AutoPacketFactory, false>;
template std::shared_ptr<AutoPacketFactory> autowiring::fast_pointer_cast<AutoPacketFactory, CoreObject>(const std::shared_ptr<CoreObject>& Other);
template class autowiring::RegType<AutoPacketFactory>;
template struct autowiring::fast_pointer_cast_blind<CoreObject, AutoPacketFactory>;
template struct autowiring::fast_pointer_cast_initializer<CoreObject, AutoPacketFactory>;
template struct autowiring::fast_pointer_cast_blind<AutoPacketFactory, CoreObject>;
template struct autowiring::fast_pointer_cast_initializer<AutoPacketFactory, CoreObject>;
template struct auto_id_t<AutoPacketFactory>;
template class auto_id_t_init<AutoPacketFactory, false>;
template class auto_id_t_init<AutoPacketFactory, true>;
