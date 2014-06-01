#include "stdafx.h"
#include "AutoPacket.h"
#include "Autowired.h"
#include "AutoPacketFactory.h"
#include "AutoPacketProfiler.h"
#include "SatCounter.h"

AutoPacket::AutoPacket(AutoPacketFactory& factory):
  m_satCounters(
    factory.GetSubscriberVector().begin(),
    factory.GetSubscriberVector().end()
  )
{
  // Prime the satisfaction graph for each element:
  for(auto& autoFilter : m_satCounters)
    for(
      auto pCur = autoFilter.GetAutoFilterInput();
      *pCur;
      pCur++
    ) {
      DecorationDisposition& entry = m_decorations[*pCur->ti];

      // Decide what to do with this entry:
      switch(pCur->subscriberType) {
      case inTypeInvalid:
        // Should never happen--trivially ignore this entry
        break;
      case inTypeRequired:
        entry.m_subscribers.push_back(std::make_pair(&autoFilter, true));
        break;
      case inTypeOptional:
        entry.m_subscribers.push_back(std::make_pair(&autoFilter, false));
        break;
      case outTypeRef:
      case outTypeRefAutoReady:
        // We don't do anything with these types.
        // Optionally, we might want to register them as outputs, or do some kind
        // of runtime detection that a multi-satisfaction case exists--but for now,
        // we just trivially ignore them.
        break;
      }
    }

  // Initial reset, required to get into a valid initial state
  Reset();
}

AutoPacket::~AutoPacket() {
  // Last chance for AutoFilter call
  ResolveOptions();

  m_decorations.clear();
  m_satCounters.clear();
}

void AutoPacket::ResolveOptions(void) {
  boost::lock_guard<boost::mutex> lk(m_lock);
  for(auto& decoration : m_decorations)
    for(auto& satCounter : decoration.second.m_subscribers)
      if(!satCounter.second)
        if(satCounter.first->Resolve())
          satCounter.first->CallAutoFilter(*this);
}

void AutoPacket::MarkUnsatisfiable(const std::type_info& info) {
  auto decoration = m_decorations.find(info);
  if(decoration == m_decorations.end())
    // Trivial return, there's no subscriber to this decoration and so we have nothing to do
    return;

  // Update everything
  for(auto& satCounter : decoration->second.m_subscribers) {
    if(satCounter.second)
      // Entry is mandatory, leave it unsatisfaible
      continue;

    // Entry is optional, we will call if we're satisfied after decrementing this optional field
    if(satCounter.first->Decrement(false))
      satCounter.first->CallAutoFilter(*this);
  }
}

void AutoPacket::UpdateSatisfaction(const std::type_info& info) {
  auto decoration = m_decorations.find(info);
  if(decoration == m_decorations.end())
    return;

  // Update everything
  for(auto& satCounter : decoration->second.m_subscribers)
    if(satCounter.first->Decrement(satCounter.second))
      satCounter.first->CallAutoFilter(*this);
}

void AutoPacket::PulseSatisfaction(DecorationDisposition* pTypeSubs[], size_t nInfos) {
  // First pass, decrement what we can:
  for(size_t i = nInfos; i--;) {
    for(auto& satCounter : pTypeSubs[i]->m_subscribers) {
      auto& cur = satCounter.first;
      if (satCounter.second) {
        --cur->remaining;
        if(!cur->remaining)
          //Call only when required data is decremented to zero
          cur->CallAutoFilter(*this);
      }
    }
  }

  // Reset all counters
  // since data in this call will not be available subsequently
  for(size_t i = nInfos; i--;) {
    for(auto& satCounter : pTypeSubs[i]->m_subscribers) {
      auto& cur = satCounter.first;
      if (satCounter.second) {
        ++cur->remaining;
      }
    }
  }
}

void AutoPacket::Reset(void) {
  // Last chance for AutoFilter call
  ResolveOptions();

  // Reset all counters:
  for(auto& satCounter : m_satCounters)
    satCounter.Reset();
  for(auto& decoration : m_decorations)
    decoration.second.Reset();

  // Initial satisfaction of the AutoPacket:
  UpdateSatisfaction(typeid(AutoPacket));
}

bool AutoPacket::HasSubscribers(const std::type_info& ti) const {
  return m_decorations.count(ti) != 0;
}
