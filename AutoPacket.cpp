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
    if(curDst.subscriber.empty() && !curSrc.GetSubscriber().empty())
      curDst.subscriber = curSrc.GetSubscriber();
  }
}

bool AutoPacket::HasSubscribers(const std::type_info& ti) const {
  auto decorator = m_factory->FindDecorator(ti);
  return !!decorator;
}