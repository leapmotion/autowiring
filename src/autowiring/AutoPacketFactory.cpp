// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoPacketFactory.h"
#include "AutoPacket.h"
#include "thread_specific_ptr.h"

AutoPacketFactory::AutoPacketFactory(void):
  ContextMember("AutoPacketFactory"),
  m_parent(GetContext()->GetParentContext()),
  m_wasStopped(false),
  m_packets(AutoPacket::CreateObjectPool(*this, m_outstanding))
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
  
  // Obtain a packet, return it
  std::shared_ptr<AutoPacket> retVal;
  m_packets(retVal);

  // Done, return
  return retVal;
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

void AutoPacketFactory::Stop(bool graceful) {
  // Return optimization
  if(m_wasStopped)
    return;

  // Kill the object pool
  m_packets.SetOutstandingLimit(0);
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
  {
    std::unique_lock<std::mutex> lk(m_lock);
    m_stateCondition.wait(lk, [this]{ return ShouldStop(); });
  }

  // Now we need to block until all packets come back to the object pool:
  m_packets.Rundown();
}

void AutoPacketFactory::Invalidate(void) {
  m_packets.ClearCachedEntities();
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
  AutoFilterDescriptor descriptor;
  //ASSUME: type_info uniquely specifies descriptor
  for (auto& af : m_autoFilters) {
    if (af.GetAutoFilterTypeInfo() == nodeType) {
      descriptor = af;
      break;
    }
  }
  //NOTE: If descriptor was not found descriptor.GetAutoFilterTypeInfo() == nullptr
  return descriptor;
}

void AutoPacketFactory::BroadcastOneDataOut(const std::type_info* nodeType, const std::type_info* dataType, bool enable) {
  std::lock_guard<std::mutex> guard(m_lock);

  // Find and copy the AutoFilterDescriptor instance.
  AutoFilterDescriptor update = GetTypeDescriptorUnsafe(nodeType);
  if (!update.GetAutoFilterTypeInfo())
    return;

  const AutoFilterDescriptorInput* argDescriptor = update.GetArgumentType(dataType);
  if (!argDescriptor ||
      !argDescriptor->isOutput()) {
    std::stringstream ss;
    ss << "Attempted to transmit broadcasts of a type " << dataType->name()
    << " that is not an output of " << nodeType->name();
    throw std::runtime_error(ss.str());
  }

  // Extract, modify and insert the broadcast state
  m_autoFilters.erase(update);
  update.Broadcast(dataType, enable);
  m_autoFilters.insert(update);

  Invalidate();
}

void AutoPacketFactory::BroadcastAllDataOut(const std::type_info* nodeType, bool enable) {
  std::lock_guard<std::mutex> guard(m_lock);

  // Find and copy the AutoFilterDescriptor instance.
  AutoFilterDescriptor update = GetTypeDescriptorUnsafe(nodeType);
  if (!update.GetAutoFilterTypeInfo())
    return;

  // Extract, modify and insert the broadcast state
  m_autoFilters.erase(update);
  // All input data types accept broadcasts
  // NOTE: Iteration is over a static array terminated with nullptr
  for (const AutoFilterDescriptorInput* pArg = update.GetAutoFilterInput(); *pArg; ++pArg) {
    if (pArg->isOutput())
      update.Broadcast(pArg->ti, enable);
  }
  m_autoFilters.insert(update);

  Invalidate();
}

void AutoPacketFactory::BroadcastOneDataIn(const std::type_info* nodeType, const std::type_info* dataType, bool enable) {
  std::lock_guard<std::mutex> guard(m_lock);

  // Find and copy the AutoFilterDescriptor instance.
  AutoFilterDescriptor update = GetTypeDescriptorUnsafe(nodeType);
  if (!update.GetAutoFilterTypeInfo())
    return;

  const AutoFilterDescriptorInput* argDescriptor = update.GetArgumentType(dataType);
  if (!argDescriptor ||
      !argDescriptor->isInput()) {
    std::stringstream ss;
    ss << "Attempted to receive broadcasts of a type " << dataType->name()
    << " that is not an input to " << nodeType->name();
    throw std::runtime_error(ss.str());
  }

  // Extract, modify and insert the broadcast state
  m_autoFilters.erase(update);
  update.Broadcast(dataType, enable);
  m_autoFilters.insert(update);

  Invalidate();
}

void AutoPacketFactory::BroadcastAllDataIn(const std::type_info* nodeType, bool enable) {
  std::lock_guard<std::mutex> guard(m_lock);

  // Find and copy the AutoFilterDescriptor instance.
  AutoFilterDescriptor update = GetTypeDescriptorUnsafe(nodeType);
  if (!update.GetAutoFilterTypeInfo())
    return;

  // Extract, modify and insert the broadcast state
  m_autoFilters.erase(update);
  // All input data types accept broadcasts
  // NOTE: Iteration is over a static array terminated with nullptr
  for (const AutoFilterDescriptorInput* pArg = update.GetAutoFilterInput(); *pArg; ++pArg) {
    if (pArg->isInput())
      update.Broadcast(pArg->ti, enable);
  }
  m_autoFilters.insert(update);

  Invalidate();
}

void AutoPacketFactory::PipeOneData(const std::type_info* nodeOutType, const std::type_info* nodeInType, const std::type_info* dataType, bool enable) {
  std::lock_guard<std::mutex> guard(m_lock);

  // Find and copy both AutoFilterDescriptor instances.
  AutoFilterDescriptor updateOut = GetTypeDescriptorUnsafe(nodeOutType);
  AutoFilterDescriptor updateIn = GetTypeDescriptorUnsafe(nodeInType);
  if (!updateOut.GetAutoFilterTypeInfo() ||
      !updateIn.GetAutoFilterTypeInfo())
    return;

  // Find both data types
  const AutoFilterDescriptorInput* argOutDescriptor = updateOut.GetArgumentType(dataType);
  const AutoFilterDescriptorInput* argInDescriptor = updateIn.GetArgumentType(dataType);
  if (!argInDescriptor ||
      !argOutDescriptor) {
    std::stringstream ss;
    ss << "Attempted to pipe data of a type " << dataType->name()
    << " that is not an an argument of both ouput " << nodeOutType->name()
    << " and input " << nodeInType->name();
    throw std::runtime_error(ss.str());
  }

  // Verify IO compatability
  if (!argOutDescriptor->isOutput() ||
      !argInDescriptor->isInput()) {
    std::stringstream ss;
    ss << "Attempted to pipe data of a type " << dataType->name()
    << " with incompatible orientations from output " << nodeOutType->name()
    << " to input " << nodeInType->name();
    throw std::runtime_error(ss.str());
  }

  // Extract, modify and insert the half-pipes
  m_autoFilters.erase(updateOut);
  m_autoFilters.erase(updateIn);
  updateOut.HalfPipe(dataType, nodeInType, enable);
  updateIn.HalfPipe(dataType, nodeOutType, enable);
  m_autoFilters.insert(updateOut);
  m_autoFilters.insert(updateIn);

  Invalidate();
}

void AutoPacketFactory::PipeAllData(const std::type_info* nodeOutType, const std::type_info* nodeInType, bool enable) {
  std::lock_guard<std::mutex> guard(m_lock);

  // Find and copy both AutoFilterDescriptor instances.
  AutoFilterDescriptor updateOut = GetTypeDescriptorUnsafe(nodeOutType);
  AutoFilterDescriptor updateIn = GetTypeDescriptorUnsafe(nodeInType);
  if (!updateOut.GetAutoFilterTypeInfo() ||
      !updateIn.GetAutoFilterTypeInfo())
    return;

  // List all correctly oriented arguments
  std::unordered_set<const std::type_info*> dataOutTypes;
  dataOutTypes.reserve(updateOut.GetArity());
  for (const AutoFilterDescriptorInput* pArg = updateOut.GetAutoFilterInput(); *pArg; ++pArg) {
    if (pArg->isOutput())
      dataOutTypes.insert(pArg->ti);
  }
  std::unordered_set<const std::type_info*> dataInTypes;
  dataInTypes.reserve(updateIn.GetArity());
  for (const AutoFilterDescriptorInput* pArg = updateIn.GetAutoFilterInput(); *pArg; ++pArg) {
    if (pArg->isInput())
      if (dataOutTypes.find(pArg->ti) != dataOutTypes.end())
        dataInTypes.insert(pArg->ti);
  }

  // Extract, modify and insert the half-pipes
  m_autoFilters.erase(updateOut);
  m_autoFilters.erase(updateIn);
  for (const std::type_info* dataType : dataInTypes) {
    updateOut.HalfPipe(dataType, nodeInType, enable);
    updateIn.HalfPipe(dataType, nodeOutType, enable);
  }
  m_autoFilters.insert(updateOut);
  m_autoFilters.insert(updateIn);

  Invalidate();
}
