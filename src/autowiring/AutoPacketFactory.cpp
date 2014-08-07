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
    m_stateCondition.wait(lk, [this]{return ShouldStop(); });
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
  m_packets.ClearCachedEntities();
}
