// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoPacket.h"
#include "Autowired.h"
#include "AutoPacketFactory.h"
#include "AutoPacketProfiler.h"
#include "AutoFilterDescriptor.h"
#include "ContextEnumerator.h"
#include "SatCounter.h"
#include <algorithm>

using namespace autowiring;

// This must appear in .cpp in order to avoid compilation failure due to:
// "Arithmetic on a pointer to an incomplete type 'SatCounter'"
AutoPacket::~AutoPacket() {}

AutoPacket::AutoPacket(AutoPacketFactory& factory, const std::shared_ptr<Object>& outstanding):
  m_outstandingRemote(outstanding)
{
  // Traverse all contexts, adding their packet subscriber vectors one at a time:
  for(const auto& curContext : ContextEnumerator(factory.GetContext())) {
    AutowiredFast<AutoPacketFactory> curFactory(curContext);
    if(curFactory)
      // Only insert if this context actually has a packet factory
      curFactory->AppendAutoFiltersTo(m_satCounters);
  }

  // Sort, eliminate duplicates
  m_satCounters.sort();
  m_satCounters.erase(std::unique(m_satCounters.begin(), m_satCounters.end()), m_satCounters.end());

  // Record divide between subscribers & recipients
  m_subscriberNum = m_satCounters.size();

  // Prime the satisfaction graph for each element:
  for(auto& satCounter : m_satCounters)
    AddSatCounter(satCounter);

  Reset();
}

void AutoPacket::AddSatCounter(SatCounter& satCounter) {
  for(auto pCur = satCounter.GetAutoFilterInput();
      *pCur;
      pCur++
      ) {
    const std::type_info& dataType = *pCur->ti;
    auto flow = satCounter.GetDataFlow(&dataType);
    if (flow.broadcast) {
      // Broadcast source is void
      DecorationDisposition* entry = &m_decorations[DSIndex(dataType, typeid(void))];
      entry->m_type = &dataType;

      // Decide what to do with this entry:
      // NOTE: Recipients added via AddReceiver can receive broadcast data,
      // so it is necessary to decrement the receiver's counters when it is added.
      if (pCur->is_input) {
        entry->m_subscribers.push_back(std::make_pair(&satCounter, !pCur->is_optional));
        if (entry->satisfied)
          satCounter.Decrement(dataType, typeid(void), !pCur->is_optional);
      }
      if (pCur->is_output) {
        if(entry->m_publisher) {
          std::stringstream ss;
          ss << "Added identical data broadcasts of type " << autowiring::demangle(pCur->ti);
          throw std::runtime_error(ss.str());
        }
        entry->m_publisher = &satCounter;
      }
    }
    for (auto halfpipe : flow.halfpipes) {
      // Pipe terminating type is defined by halfpipe
      DecorationDisposition* entry;
      if (flow.output) {
        entry = &m_decorations[DSIndex(dataType, *satCounter.GetType())];
      } else {
        entry = &m_decorations[DSIndex(dataType, halfpipe)];
      }
      entry->m_type = &dataType;

      // Decide what to do with this entry:
      // NOTE: Recipients added via AddReceiver cannot receive piped data,
      // and subscribers are added before the packet is decorated.
      if (pCur->is_input) {
        entry->m_subscribers.push_back(std::make_pair(&satCounter, !pCur->is_optional));
      }
      if (pCur->is_output) {
        if(entry->m_publisher) {
          std::stringstream ss;
          ss << "Added identical data pipes from " << satCounter.GetAutoFilterTypeInfo()->name() << " of type " << pCur->ti->name();
          throw std::runtime_error(ss.str());
        }
        entry->m_publisher = &satCounter;
      }
    }
  }
}

void AutoPacket::RemoveSatCounter(SatCounter& satCounter) {
  for(auto pCur = satCounter.GetAutoFilterInput();
      *pCur;
      pCur++
      ) {
    const std::type_info& dataType = *pCur->ti;
    auto flow = satCounter.GetDataFlow(&dataType);
    if (flow.broadcast) {
      // Broadcast source is void
      DecorationDisposition* entry = &m_decorations[DSIndex(dataType, typeid(void))];

      // Decide what to do with this entry:
      if (pCur->is_input) {
        assert(!entry->m_subscribers.empty());
        assert(&satCounter == entry->m_subscribers.back().first);
        entry->m_subscribers.pop_back();
      }
      if (pCur->is_output) {
        assert(&satCounter == entry->m_publisher);
        entry->m_publisher = nullptr;
      }
    }
    for (auto halfpipe : flow.halfpipes) {
      // Pipe terminating type is defined by halfpipe
      DecorationDisposition* entry;
      if (flow.output) {
        entry = &m_decorations[DSIndex(dataType, *satCounter.GetType())];
      } else {
        entry = &m_decorations[DSIndex(dataType, halfpipe)];
      }

      // Decide what to do with this entry:
      if (pCur->is_input) {
        assert(!entry->m_subscribers.empty());
        assert(&satCounter == entry->m_subscribers.back().first);
        entry->m_subscribers.pop_back();
      }
      if (pCur->is_output) {
        assert(&satCounter == entry->m_publisher);
        entry->m_publisher = nullptr;
      }
    }
  }
}

ObjectPool<AutoPacket> AutoPacket::CreateObjectPool(AutoPacketFactory& factory, const std::shared_ptr<Object>& outstanding) {
  return ObjectPool<AutoPacket>(
    ~0,
    ~0,
    [&factory, &outstanding] { return new AutoPacket(factory, outstanding); },
    [] (AutoPacket& packet) { packet.Initialize(); },
    [] (AutoPacket& packet) {
      // IMPORTANT: Create shared_ptr with no destructor to enable outputs to AutoPacket
      std::shared_ptr<AutoPacket> shared(&packet, [](AutoPacket*){});
      packet.Finalize();
    }
  );
}

void AutoPacket::MarkUnsatisfiable(const std::type_info& info, const std::type_info& source) {
  std::list<SatCounter*> callQueue;
  {
    std::lock_guard<std::mutex> lk(m_lock);
    auto dFind = m_decorations.find(DSIndex(info, source));
    if(dFind == m_decorations.end())
      // Trivial return, there's no subscriber to this decoration and so we have nothing to do
      return;

    // Update satisfaction inside of lock
    DecorationDisposition* decoration = &dFind->second;
    for(const auto& satCounter : decoration->m_subscribers) {
      if(satCounter.second)
        // Entry is mandatory, leave it unsatisfaible
        continue;

      // Entry is optional, we will call if we're satisfied after decrementing this optional field
      if(satCounter.first->Decrement(info, source, false))
        callQueue.push_back(satCounter.first);
    }
  }

  // Make calls outside of lock, to avoid deadlock from decorations in methods
  for (SatCounter* call : callQueue)
    call->CallAutoFilter(*this);
}

void AutoPacket::UpdateSatisfaction(const std::type_info& info, const std::type_info& source) {
  std::list<SatCounter*> callQueue;
  {
    std::lock_guard<std::mutex> lk(m_lock);

    if (info != typeid(auto_arg<const AutoPacket&>::id_type)) {
      switch (m_lifecyle) {
        case disable_decorate: throw std::runtime_error("Cannot provide decorations in final-call (const AutoPacket&) AutoFilter methods");
        case disable_update: return; // Quietly prevent recusion during optional_ptr resolution
        default: break; //enable_all
      }
    }

    auto dFind = m_decorations.find(DSIndex(info, source));
    if(dFind == m_decorations.end())
      // Trivial return, there's no subscriber to this decoration and so we have nothing to do
      return;

    // Update satisfaction inside of lock
    DecorationDisposition* decoration = &dFind->second;
    for(const auto& satCounter : decoration->m_subscribers)
      if(satCounter.first->Decrement(info, source, satCounter.second))
        callQueue.push_back(satCounter.first);
  }

  // Make calls outside of lock, to avoid deadlock from decorations in methods
  for (SatCounter* call : callQueue)
    call->CallAutoFilter(*this);
}

void AutoPacket::PulseSatisfaction(DecorationDisposition* pTypeSubs[], size_t nInfos) {
  // TODO: DecorateImmediate can only broadcast - change this to allow sourced immediate decoration.
  const std::type_info& source = typeid(void);

  std::list<SatCounter*> callQueue;
  // First pass, decrement what we can:
  {
    std::lock_guard<std::mutex> lk(m_lock);
    switch (m_lifecyle) {
      case disable_decorate: throw std::runtime_error("Cannot provide decorations in final-call (const AutoPacket&) AutoFilter methods");
      case disable_update: return; // Quietly prevent recusion during optional_ptr resolution
      default: break; //enable_all
    }

    for(size_t i = nInfos; i--;) {
      for(std::pair<SatCounter*, bool>& subscriber : pTypeSubs[i]->m_subscribers) {
        SatCounter* cur = subscriber.first;
        if(
          // We only care about mandatory inputs
          subscriber.second &&

          // We only care about sat counters that aren't deferred--skip everyone else
          // Deferred calls will be too late.
          !cur->IsDeferred() &&

          // We cannot satisfy shared_ptr arguments, since the implied existence
          // guarantee of shared_ptr is violated
          !cur->GetArgumentType(pTypeSubs[i]->m_type)->is_shared &&

          // Now do the decrementation and proceed even if optional > 0,
          // since this is the only opportunity to fulfill the arguments
          (cur->Decrement(*pTypeSubs[i]->m_type, source, true) ||
           cur->remaining == 0)
        )
          // Finally, queue a call for this type
          callQueue.push_back(cur);
      }
    }
  }

  // Make calls outside of lock, to avoid deadlock from decorations in methods
  for (SatCounter* call : callQueue)
    call->CallAutoFilter(*this);

  // Reset all counters
  // since data in this call will not be available subsequently
  {
    std::lock_guard<std::mutex> lk(m_lock);
    for(size_t i = nInfos; i--;) {
      for(const auto& satCounter : pTypeSubs[i]->m_subscribers) {
        SatCounter* cur = satCounter.first;
        if (satCounter.second) {
          cur->Increment(*pTypeSubs[i]->m_type, source, true);
        }
      }
    }
  }
}

bool AutoPacket::UnsafeHas(const std::type_info& data, const std::type_info& source) const {
  auto q = m_decorations.find(DSIndex(data, source));
  if(q == m_decorations.end())
    return false;
  return q->second.satisfied;
}

void AutoPacket::UnsafeCheckout(AnySharedPointer* ptr, const std::type_info& data, const std::type_info& source) {
  autowiring::DataFlow flow = GetDataFlow(data, source);
  if (flow.broadcast) {
    auto& entry = m_decorations[DSIndex(data, typeid(void))];
    entry.m_type = &data; // Ensure correct type if instantiated here

    if (entry.satisfied) {
      std::stringstream ss;
      ss << "Cannot decorate this packet with type " << autowiring::demangle(*ptr)
      << ", the requested decoration already exists";
      throw std::runtime_error(ss.str());
    }
    if(entry.isCheckedOut) {
      std::stringstream ss;
      ss << "Cannot check out decoration of type " << autowiring::demangle(*ptr)
      << ", it is already checked out elsewhere";
      throw std::runtime_error(ss.str());
    }
    entry.isCheckedOut = true;
    entry.m_decoration = *ptr;
  }
  if (!flow.halfpipes.empty() ||
      !flow.broadcast) {
    auto& entry = m_decorations[DSIndex(data, source)];
    entry.m_type = &data; // Ensure correct type if instantiated here

    if (entry.satisfied) {
      std::stringstream ss;
      ss << "Cannot decorate this packet with type " << autowiring::demangle(*ptr)
      << ", the requested decoration already exists";
      throw std::runtime_error(ss.str());
    }
    if(entry.isCheckedOut) {
      std::stringstream ss;
      ss << "Cannot check out decoration of type " << autowiring::demangle(*ptr)
      << ", it is already checked out elsewhere";
      throw std::runtime_error(ss.str());
    }
    entry.isCheckedOut = true;
    entry.m_decoration = *ptr;
  }
}

void AutoPacket::UnsafeComplete(bool ready, const std::type_info& data, const std::type_info& source,
                                DecorationDisposition* &broadDeco, DecorationDisposition* &pipedDeco) {
  autowiring::DataFlow flow = GetDataFlow(data, source);
  if (flow.broadcast) {
    broadDeco = &m_decorations[DSIndex(data, typeid(void))];

    assert(broadDeco->m_type != nullptr); // CompleteCheckout must be for an initialized DecorationDisposition
    assert(broadDeco->isCheckedOut); // CompleteCheckout must follow Checkout

    if(!ready)
      // Memory must be released, the checkout was cancelled
      broadDeco->m_decoration->reset();

    // Reset the checkout flag before releasing the lock:
    broadDeco->isCheckedOut = false;
    broadDeco->satisfied = true;
  }
  if (!flow.halfpipes.empty() ||
      !flow.broadcast) {
    // IMPORTANT: If data isn't broadcast it should be provided with a source.
    // This enables extraction of multiple types without collision.
    pipedDeco = &m_decorations[DSIndex(data, source)];

    assert(pipedDeco->m_type != nullptr); // CompleteCheckout must be for an initialized DecorationDisposition
    assert(pipedDeco->isCheckedOut); // CompleteCheckout must follow Checkout

    if(!ready)
      // Memory must be released, the checkout was cancelled
      pipedDeco->m_decoration->reset();

    // Reset the checkout flag before releasing the lock:
    pipedDeco->isCheckedOut = false;
    pipedDeco->satisfied = true;
  }
}

void AutoPacket::ForwardAll(std::shared_ptr<AutoPacket> recipient) const {
  std::list<DecorationDisposition*> decoQueue;
  {
    // Use memory well-ordering to establish a lock heirarchy
    if(this < recipient.get()) {
      m_lock.lock();
      recipient->m_lock.lock();
    }
    else {
      recipient->m_lock.lock();
      m_lock.lock();
    }

    for (auto& decoration : m_decorations) {
      // Only existing data is propagated
      // Unsatisfiable quilifiers are NOT propagated
      // ASSERT: AutoPacket types are never marked "satisfied"
      if (!decoration.second.satisfied)
        continue;

      // Only broadcast data is propagated
      const std::type_info& source(typeid(void));
      if (std::get<1>(decoration.first) != source)
        continue;

      const std::type_info& data = *decoration.second.m_type;

      // Quietly drop data that is already present on recipient
      if (recipient->UnsafeHas(data, typeid(void)))
        continue;

      AnySharedPointer any_ptr(decoration.second.m_decoration);
      recipient->UnsafeCheckout(&any_ptr, data, source);

      DecorationDisposition* broadDeco = nullptr;
      DecorationDisposition* pipedDeco = nullptr;
      recipient->UnsafeComplete(true, data, source, broadDeco, pipedDeco);

      decoQueue.push_back(broadDeco);
    }

    m_lock.unlock();
    recipient->m_lock.unlock();
  }

  // Recipient satisfaction is updated outside of lock
  for (DecorationDisposition* broadDeco : decoQueue) {
    // Satisfy the base declaration first and then the shared pointer:
    recipient->UpdateSatisfaction(broadDeco->m_decoration->type(), typeid(void));
  }
}

DataFlow AutoPacket::GetDataFlow(const DecorationDisposition& entry) const {
  DataFlow flow; //DEFAULT: No broadcast, no pipes
  if (!entry.m_publisher) {
    // Broadcast is always true for added or snooping recipients
    flow.broadcast = true;
  } else {
    flow = entry.m_publisher->GetDataFlow(entry.m_type);
  }
  return flow;
}

DataFlow AutoPacket::GetDataFlow(const std::type_info& data, const std::type_info& source) {
  DataFlow flow; //DEFAULT: No pipes
  flow.broadcast = true; //DEFAULT: Broadcast data from anonymous sources
  size_t sat_ind = 0;
  for (auto& sat : m_satCounters) {
    if (sat_ind > m_subscriberNum)
      break;
    if (&source == sat.GetAutoFilterTypeInfo())
      flow = sat.GetDataFlow(&data);
    ++sat_ind;
  }
  return flow;
}

void AutoPacket::Reset(void) {
  // Initialize all counters:
  std::lock_guard<std::mutex> lk(m_lock);
  for(auto& satCounter : m_satCounters)
    satCounter.Reset();

  // Clear all references:
  for(auto& decoration : m_decorations)
    decoration.second.Reset();
}

void AutoPacket::Initialize(void) {
  // Hold an outstanding count from the parent packet factory
  m_outstanding = m_outstandingRemote;
  if(!m_outstanding)
    throw std::runtime_error("Cannot proceed with this packet, enclosing context already expired");

  // Enter issued state
  m_lifecyle = enable_all;

  // Find all subscribers with no required or optional arguments:
  std::list<SatCounter*> callCounters;
  for (auto& satCounter : m_satCounters)
    if (satCounter)
      callCounters.push_back(&satCounter);

  // Call all subscribers with no required or optional arguments:
  // NOTE: This may result in decorations that cause other subscribers to be called.
  for (SatCounter* call : callCounters)
    call->CallAutoFilter(*this);

  // First-call indicated by argumument type AutoPacket&:
  UpdateSatisfaction(typeid(auto_arg<AutoPacket&>::id_type));
}

void AutoPacket::Finalize(void) {
  // Queue calls to ensure that calls to Decorate inside of AutoFilter methods
  // will NOT effect the resolution of optional arguments.
  std::list<SatCounter*> callQueue;
  {
    std::lock_guard<std::mutex> lk(m_lock);
    for(auto& decoration : m_decorations)
      for(auto& satCounter : decoration.second.m_subscribers)
        if(!satCounter.second)
          if(satCounter.first->Resolve())
            callQueue.push_back(satCounter.first);
  }
  m_lifecyle = disable_update;
  for (SatCounter* call : callQueue)
    call->CallAutoFilter(*this);

  // Last-call indicated by argumument type const AutoPacket&:
  m_lifecyle = disable_decorate;
  UpdateSatisfaction(typeid(auto_arg<const AutoPacket&>::id_type));

  // Remove all recipients & clean up the decorations list
  // ASSERT: This reverses the order of accumulation,
  // so searching for the subscriber is avoided.
  while (m_satCounters.size() > m_subscriberNum) {
    SatCounter& recipient = m_satCounters.back();
    RemoveSatCounter(recipient);
    m_satCounters.pop_back();
  }

  // Remove decoration dispositions specific to subscribers
  t_decorationMap::iterator dItr = m_decorations.begin();
  t_decorationMap::iterator dEnd = m_decorations.end();
  while (dItr != dEnd) {
    if (dItr->second.m_subscribers.empty())
      dItr = m_decorations.erase(dItr);
    else
      ++dItr;
  }

  Reset();
}

void AutoPacket::InitializeRecipient(const AutoFilterDescriptor& descriptor) {
  SatCounter* call = nullptr;
  {
    std::lock_guard<std::mutex> lk(m_lock);

    // (1) Append & Initialize new satisfaction counter
    m_satCounters.push_back(descriptor);
    SatCounter& recipient = m_satCounters.back();
    recipient.Reset();

    // (2) Update satisfaction & Append types from subscriber
    AddSatCounter(recipient);

    // (3) Check call status inside of lock
    if (recipient) {
      call = &recipient;
    }
  }

  // (4) If all types are satisfied, call AutoFilter now.
  if (call)
    call->CallAutoFilter(*this);
}

SatCounter AutoPacket::GetSatisfaction(const std::type_info& subscriber) const {
  std::lock_guard<std::mutex> lk(m_lock);
  for (auto& sat : m_satCounters)
    if (sat.GetType() == &subscriber)
      return sat;
  return SatCounter();
}

std::list<SatCounter> AutoPacket::GetSubscribers(const std::type_info& data, const std::type_info& source) const {
  std::lock_guard<std::mutex> lk(m_lock);
  std::list<SatCounter> subscribers;
  t_decorationMap::const_iterator decoration = m_decorations.find(DSIndex(data, source));
  if (decoration != m_decorations.end())
    for (auto& subscriber : decoration->second.m_subscribers)
      subscribers.push_back(*subscriber.first);
  return subscribers;
}

std::list<DecorationDisposition> AutoPacket::GetDispositions(const std::type_info& data) const {
  std::lock_guard<std::mutex> lk(m_lock);
  std::list<DecorationDisposition> dispositions;
  for (auto& disposition : m_decorations)
    if (disposition.second.m_type == &data)
      dispositions.push_back(disposition.second);
  return dispositions;
}

bool AutoPacket::HasSubscribers(const std::type_info& data, const std::type_info& source) const {
  std::lock_guard<std::mutex> lk(m_lock);
  return m_decorations.count(DSIndex(data, source)) != 0;
}
