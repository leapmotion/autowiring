#include "stdafx.h"
#include "AutoPacket.h"
#include "AutoPacketFactory.h"
#include <boost/any.hpp>

AutoPacketSubscriberStatus::AutoPacketSubscriberStatus(const AutoPacketSubscriber& subscriber) :
  m_pSubscriber(&subscriber),
  m_visitCount(subscriber.GetDegree())
{
}

AutoPacket::AutoPacket()
{
}

AutoPacket::~AutoPacket()
{
}

void AutoPacket::UpdateSatisfaction(const std::type_info& info) {
  size_t subscriberIndex = m_factory->FindSubscriberIndex(info);
  if(subscriberIndex == -1)
    return;

  // Obtain the corresponding index:
  if(!m_satCounters[subscriberIndex]--)
    __noop();
}

void AutoPacket::Reset(void) {
  auto& vec = m_factory->GetSubscriberVector();
  m_satCounters.resize(vec.size());
  for(size_t i = vec.size(); i--; )
    m_satCounters[i] = vec[i];
}