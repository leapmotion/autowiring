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
    if(!--m_satCounters[*q])
      satVec[*q].Call(*this);
}

void AutoPacket::Reset(void) {
  auto& vec = m_factory->GetSubscriberVector();
  m_satCounters.resize(vec.size());
  for(size_t i = vec.size(); i--; )
    m_satCounters[i] = vec[i].GetArity();
}