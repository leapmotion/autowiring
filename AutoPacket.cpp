#include "stdafx.h"
#include "AutoPacket.h"
#include "AutoPacketFactory.h"
#include <boost/any.hpp>

AutoPacket::AutoPacket()
{
}

AutoPacket::~AutoPacket()
{
}

void AutoPacket::UpdateSatisfaction(const std::type_info& info) {
  auto decorator = m_factory->FindDecorator(info);
  if(!decorator)
    return;

  // Update all satisfaction counters:
  const auto& satVec = m_factory->GetSubscriberVector();
  const auto& subscribers = decorator->subscribers;
  for(auto q = subscribers.begin(); q != subscribers.end(); q++)
    if(!--m_satCounters[*q].remaining) {
      // No entries remaining in this saturation, we can now make the call
      auto& entry = satVec[*q];
      assert(entry.GetCall());
      entry.GetCall()(entry.GetSubscriberPtr(), *this);
    }
}

void AutoPacket::Release(void) {
  for(size_t i = m_satCounters.size(); i--;)
    m_satCounters[i].subscriber = boost::any();
}

void AutoPacket::Reset(void) {
  for(auto q = m_mp.begin(); q != m_mp.end(); q++)
    if(q->second) {
      delete q->second;
      q->second = nullptr;
    }

  auto& vec = m_factory->GetSubscriberVector();
  m_satCounters.resize(vec.size());
  for(size_t i = vec.size(); i--;) {
    const auto& curSrc = vec[i];
    auto& curDst = m_satCounters[i];

    curDst.remaining = curSrc.GetArity();
    curDst.subscriber = curSrc.GetSubscriber();
  }
}

bool AutoPacket::HasSubscribers(const std::type_info& ti) const {
  // Obtain the decorator:
  auto decorator = m_factory->FindDecorator(ti);
  if(!decorator)
    // Nobody anywhere cares about this type
    return false;

  const auto& subscribers = decorator->subscribers;
  for(size_t i = subscribers.size(); i--; )
    if(
      subscribers[i] < m_satCounters.size() &&
      m_satCounters[subscribers[i]].remaining
    )
      // Found a valid, enabled subscriber
      return true;

  // No matches, end here
  return false;
}