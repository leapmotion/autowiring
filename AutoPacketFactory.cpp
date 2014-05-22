#include "stdafx.h"
#include "AutoPacketFactory.h"
#include "AutoPacket.h"

AutoPacket* AutoPacketFactory::AutoPacketCreator::operator()() const {
  return new AutoPacket(*factory);
}

AutoPacketFactory::AutoPacketFactory(void):
  m_wasStopped(false),
  m_packets(
    ~0,
    ~0,
    [this] { return new AutoPacket(*this); },
    [] (AutoPacket& packet) { packet.Reset(); }
  )
{
}

AutoPacketFactory::~AutoPacketFactory() {}

std::shared_ptr<AutoPacket> AutoPacketFactory::NewPacket(void) {
  if (!IsRunning())
    throw autowiring_error("Cannot create a packet until the AutoPacketFactory is started");
  
  // Obtain a packet, return it
  std::shared_ptr<AutoPacket> retVal;
  m_packets(retVal);

  // Done, return
  return retVal;
}

bool AutoPacketFactory::Start(std::shared_ptr<Object> outstanding) {
  boost::lock_guard<boost::mutex> lk(m_lock);
  m_outstanding = outstanding;
  
  m_stateCondition.notify_all();
  return true;
}

void AutoPacketFactory::Stop(bool graceful) {
  // Release any external references before obtaining the lock
  m_autoFilters.clear();

  // Kill the object pool
  m_packets.SetOutstandingLimit(0);
  m_packets.ClearCachedEntities();

  // Now we can lock, update state, and notify any listeners
  boost::lock_guard<boost::mutex> lk(m_lock);
  m_wasStopped = true;
  m_outstanding.reset();
  m_stateCondition.notify_all();
}

void AutoPacketFactory::Wait(void) {
  {
    boost::unique_lock<boost::mutex> lk(m_lock);
    m_stateCondition.wait(lk, [this]{return ShouldStop(); });
  }

  // Now we need to block until all packets come back to the object pool:
  m_packets.Rundown();
}

void AutoPacketFactory::AddSubscriber(const AutoFilterDescriptor& rhs) {
  (boost::lock_guard<boost::mutex>)m_lock,
  m_autoFilters.insert(rhs);

  // Trigger object pool reset after releasing the lock.  While it's possible that some
  // packets may be issued between lock reset and object pool reset, these packets will
  // not be specifically invalid; they will simply result in late delivery to certain
  // recipients.  Eventually, all packets will be reset and released.
  m_packets.ClearCachedEntities();
}

void AutoPacketFactory::RemoveSubscriber(const AutoFilterDescriptor& autoFilter) {
  // Trivial removal from the autofilter set:
  {
    boost::lock_guard<boost::mutex> lk(m_lock);
    m_autoFilters.erase(autoFilter);
  }

  // Regeneration of the packet pool for the same reason as described in AddSubscriber
  m_packets.ClearCachedEntities();
}
