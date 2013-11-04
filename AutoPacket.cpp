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
    if(!--m_satCounters[*q]) {
      auto& entry = satVec[*q];
      entry.GetCall()(entry.GetSubscriber(), *this);
    }
}

void AutoPacket::Reset(void) {
  for(auto q = m_mp.begin(); q != m_mp.end(); q++)
    if(q->second) {
      delete q->second;
      q->second = nullptr;
    }

  auto& vec = m_factory->GetSubscriberVector();
  m_satCounters.resize(vec.size());
  for(size_t i = vec.size(); i--; )
    m_satCounters[i] = vec[i].GetArity();
}

bool AutoPacket::HasSubscribers(const std::type_info& ti) const {
  auto decorator = m_factory->FindDecorator(ti);
  return !!decorator;
}