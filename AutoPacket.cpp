#include "stdafx.h"
#include "AutoPacket.h"
#include "AutoPacketFactory.h"
#include "AutoPacketProfiler.h"
#include <boost/any.hpp>

AutoPacket::AutoPacket()
{
}

AutoPacket::~AutoPacket()
{
}

void AutoPacket::UpdateSatisfactionSpecific(size_t subscriberIndex) {
  // No entries remaining in this saturation, we can now make the call
  const auto& satVec = m_factory->GetSubscriberVector();
  auto& entry = satVec[subscriberIndex];
  assert(entry.GetCall());

  if(m_profiler && m_profiler->ShouldProfile()) {
    // Record the current time before we hand control over to the call:
    auto before = boost::chrono::high_resolution_clock::now();

    // Make the actual call
    entry.GetCall()(entry.GetSubscriberPtr(), *this);

    // Record the total time spent in the call
    // TODO:  This naive implementation will also add any time spent in
    // subscribers which are reached from the subscriber defined above.
    // A good profiling system should exclude the time spent in dependent
    // subscribers by having some sort of thread-specific reentrancy
    // detection on this function--perhaps by propagating out a "boost"
    // value which will be subtracted by callers operating at higher
    // elevation levels.
    m_profiler->AddProfilingInformation(
      *entry.GetSubscriberTypeInfo(),
      boost::chrono::high_resolution_clock::now() - before
      );
  }
  else
    // No profiling required, just make the call directly
    entry.GetCall()(entry.GetSubscriberPtr(), *this);
}

void AutoPacket::UpdateSatisfaction(const std::type_info& info) {
  auto decorator = m_factory->FindDecorator(info);
  if(!decorator)
    // Trivial return, there's no subscriber to this decoration
    return;

  // Update all satisfaction counters:
  const auto& subscribers = decorator->subscribers;
  for(size_t i = subscribers.size(); i--;) {
    const auto& subscriber = subscribers[i];
    if(!m_satCounters[subscriber.first].Decrement(subscriber.second))
      UpdateSatisfactionSpecific(subscriber.first);
  }
}

void AutoPacket::RevertSatisfaction(const std::type_info& info) {
  Object* pObj;
  {
    boost::lock_guard<boost::mutex> lk(m_lock);
    auto q = m_mp.find(info);
    if(q == m_mp.end())
      return;
    pObj = q->second;
    m_mp.erase(q);
  }
  if(pObj)
    delete pObj;
}

void AutoPacket::Release(void) {
  for(size_t i = m_satCounters.size(); i--;) {
    auto& satCounter = m_satCounters[i];

    // Scan through the saturation counters, looking for any where the mandatory
    // count is satisfied but the optional count is not:
    if(satCounter.optional && !satCounter.remaining)
      // We can satisfy this one now:
      UpdateSatisfactionSpecific(i);

    // Reset this subscriber unconditionally:
    satCounter.subscriber = boost::any();
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

    curDst.remaining = curSrc.GetArity() - curSrc.GetOptionalCount();
    curDst.optional = curSrc.GetOptionalCount();
    curDst.subscriber = curSrc.GetSubscriber();
  }

  // We will automatically satisfy any requests for AutoPacket:
  UpdateSatisfaction(typeid(AutoPacket));
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
      subscribers[i].first < m_satCounters.size() &&
      m_satCounters[subscribers[i].first].remaining
    )
      // Found a valid, enabled subscriber
      return true;

  // No matches, end here
  return false;
}