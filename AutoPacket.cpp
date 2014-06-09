#include "stdafx.h"
#include "AutoPacket.h"
#include "Autowired.h"
#include "AutoPacketFactory.h"
#include "AutoPacketProfiler.h"
#include "SatCounter.h"
#include <list>

AutoPacket::AutoPacket(AutoPacketFactory& factory):
  m_satCounters(
    factory.GetSubscriberVector().begin(),
    factory.GetSubscriberVector().end()
  )
{
  // Prime the satisfaction graph for each element:
  for(auto& satCounter : m_satCounters) {
    for(
      auto pCur = satCounter.GetAutoFilterInput();
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
        entry.m_subscribers.push_back(std::make_pair(&satCounter, true));
        break;
      case inTypeOptional:
        entry.m_subscribers.push_back(std::make_pair(&satCounter, false));
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
  }
  Initialize();
}

AutoPacket::~AutoPacket() {
  // Last chance for AutoFilter call
  ResolveOptions();
}

ObjectPool<AutoPacket> AutoPacket::CreateObjectPool(AutoPacketFactory& factory) {
  return ObjectPool<AutoPacket>(
    ~0,
    ~0,
    [&factory] { return new AutoPacket(factory); },
    [] (AutoPacket& packet) {
      // Last chance for AutoFilter optional calls
      packet.ResolveOptions();

      // Reinitialize the whole packet:
      packet.Initialize();
    }
  );
}

void AutoPacket::ResolveOptions(void) {
  // Queue calls to ensure that calls to Decorate inside of AutoFilter methods
  // will NOT effect the resolution of optional arguments.
  std::list<SatCounter*> callQueue;
  {
    boost::lock_guard<boost::mutex> lk(m_lock);
    for(auto& decoration : m_decorations)
      for(auto& satCounter : decoration.second.m_subscribers)
        if(!satCounter.second)
          if(satCounter.first->Resolve())
            callQueue.push_back(satCounter.first);
  }
  for (SatCounter* call : callQueue)
    call->CallAutoFilter(*this);
}

void AutoPacket::MarkUnsatisfiable(const std::type_info& info) {
  DecorationDisposition* decoration;
  {
    boost::lock_guard<boost::mutex> lk(m_lock);
    auto dFind = m_decorations.find(info);
    if(dFind == m_decorations.end())
      // Trivial return, there's no subscriber to this decoration and so we have nothing to do
      return;
    decoration = &dFind->second;
  }

  // Update everything
  for(auto& satCounter : decoration->m_subscribers) {
    if(satCounter.second)
      // Entry is mandatory, leave it unsatisfaible
      continue;

    // Entry is optional, we will call if we're satisfied after decrementing this optional field
    if(satCounter.first->Decrement(false))
        satCounter.first->CallAutoFilter(*this);
  }
}

void AutoPacket::UpdateSatisfaction(const std::type_info& info) {
  DecorationDisposition* decoration;
  {
    boost::lock_guard<boost::mutex> lk(m_lock);
    auto dFind = m_decorations.find(info);
    if(dFind == m_decorations.end())
      // Trivial return, there's no subscriber to this decoration and so we have nothing to do
      return;
    decoration = &dFind->second;
  }

  // Update everything
  for(auto& satCounter : decoration->m_subscribers)
    if(satCounter.first->Decrement(satCounter.second))
        satCounter.first->CallAutoFilter(*this);
}

void AutoPacket::PulseSatisfaction(DecorationDisposition* pTypeSubs[], size_t nInfos) {
  // First pass, decrement what we can:
  for(size_t i = nInfos; i--;) {
    for(std::pair<SatCounter*, bool>& subscriber : pTypeSubs[i]->m_subscribers) {
      SatCounter* cur = subscriber.first;
      if(
        // We only care about mandatory inputs
        subscriber.second &&

        // We only care about sat counters that aren't deferred--skip everyone else
        // Deferred calls will be too late.
        !cur->IsDeferred() &&

        // Now do the decrementation, and only proceed if the decremented value is zero
        !--cur->remaining
      )
        // Finally, a call is safe to make on this type
        cur->CallAutoFilter(*this);
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

void AutoPacket::Initialize(void) {
  // Initialize all counters:
  {
    boost::lock_guard<boost::mutex> lk(m_lock);
    for(auto& satCounter : m_satCounters)
      satCounter.Reset();
    for(auto& decoration : m_decorations)
      decoration.second.Reset();
  }

  // Call all subscribers with no required or optional arguments:
  for (auto& satCounter : m_satCounters)
    if (satCounter)
      satCounter.CallAutoFilter(*this);

  // Initial satisfaction of the AutoPacket:
  UpdateSatisfaction(typeid(AutoPacket));
}

bool AutoPacket::HasSubscribers(const std::type_info& ti) const {
  return m_decorations.count(ti) != 0;
}
