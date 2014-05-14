#include "stdafx.h"
#include "AutoPacket.h"
#include "Autowired.h"
#include "AutoPacketFactory.h"
#include "AutoPacketProfiler.h"
#include <boost/any.hpp>

AutoPacket::AutoPacket(AutoPacketFactory& factory):
  m_factory(factory)
{}

AutoPacket::~AutoPacket() {}

void AutoPacket::UpdateSatisfactionSpecific(size_t subscriberIndex) {
  // No entries remaining in this saturation, we can now make the call
  const auto& satVec = m_factory.GetSubscriberVector();
  auto& entry = satVec[subscriberIndex];
  assert(entry.GetCall());

  AutowiredFast<AutoPacketProfiler> profiler;
  if(profiler && profiler->ShouldProfile()) {
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
    profiler->AddProfilingInformation(
      *entry.GetSubscriberTypeInfo(),
      boost::chrono::high_resolution_clock::now() - before
    );
  }
  else
    // No profiling required, just make the call directly
    entry.GetCall()(entry.GetSubscriberPtr(), *this);
}

void AutoPacket::UpdateSatisfaction(const std::type_info& info, bool is_satisfied) {
  auto decorator = m_factory.FindDecorator(info);
  if(!decorator)
    // Trivial return, there's no subscriber to this decoration
    return;

  // Update all satisfaction counters:
  const auto& subscribers = decorator->subscribers;
  for(size_t i = subscribers.size(); i--;) {
    const auto& subscriber = subscribers[i];

    if(m_satCounters.size() <= subscriber.first)
      // Can't do anything about this, subscriber went away before we could get them a packet.
      // Continue on to the next subscriber.
      continue;

    if(m_satCounters[subscriber.first])
      // Ignore this entry, no matter what happens, something already caused it to be satisfied
      continue;

    if(!is_satisfied && !subscriber.second)
      // Not satisfied, but this entry is not optional--we have to cancel and circle around
      continue;

    if(!m_satCounters[subscriber.first].Decrement(subscriber.second))
      UpdateSatisfactionSpecific(subscriber.first);
  }
}

void AutoPacket::PulseSatisfaction(const std::type_info& info) {
  auto decorator = m_factory.FindDecorator(info);
  if(!decorator)
    return;

  // Roll back all satisfaction counters:
  const auto& subscribers = decorator->subscribers;
  const auto& subscriberDescriptors = m_factory.GetSubscriberVector();
  for(size_t i = subscribers.size(); i--;) {
    const auto& subscriber = subscribers[i];

    if(subscriberDescriptors[subscriber.second].IsDeferred())
      // Deferred subscriber, nothing we can do here.
      continue;

    auto& curCounter = m_satCounters[subscriber.first];
    if(!curCounter.PseudoDecrement(subscriber.second))
      // Still has entries outstanding, give up
      continue;

    // Commit this decrementation, generate the call
    curCounter.Decrement(subscriber.second);
    UpdateSatisfactionSpecific(subscriber.first);
  }
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

  // Now that all second chances are notified, we will release held objects:
  for(auto q = m_mp.begin(); q != m_mp.end(); q++)
    q->second.Release();
}

void AutoPacket::Reset(void) {
  for(auto q = m_mp.begin(); q != m_mp.end(); q++)
    q->second.satisfied = false;

  auto& vec = m_factory.GetSubscriberVector();
  m_satCounters.resize(vec.size());
  for(size_t i = vec.size(); i--;) {
    const auto& curSrc = vec[i];
    auto& curDst = m_satCounters[i];

    curDst.remaining = curSrc.GetRequiredCount();
    curDst.optional = curSrc.GetOptionalCount();
    curDst.subscriber = curSrc.GetSubscriber();
  }

  // We will automatically satisfy any requests for AutoPacket:
  UpdateSatisfaction(typeid(AutoPacket), true);
}

bool AutoPacket::HasSubscribers(const std::type_info& ti) const {
  // Obtain the decorator:
  auto decorator = m_factory.FindDecorator(ti);
  if(!decorator)
    // Nobody anywhere cares about this type
    return false;

  const auto& subscribers = decorator->subscribers;
  for(size_t i = subscribers.size(); i--; ) {
    if(subscribers[i].first >= m_satCounters.size())
      continue;

    const auto& cur = m_satCounters[subscribers[i].first];
    if(cur.remaining || cur.optional)
      // Found a valid, enabled subscriber
      return true;
  }

  // No matches, end here
  return false;
}

std::shared_ptr<AutoPacket> ExtractSharedPointer(const AutoPacketAdaptor& adaptor) {
  return adaptor;
}