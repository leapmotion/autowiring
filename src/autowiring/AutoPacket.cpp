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

AutoPacket::AutoPacket(AutoPacketFactory& factory, std::shared_ptr<void>&& outstanding):
  m_parentFactory(std::static_pointer_cast<AutoPacketFactory>(factory.shared_from_this())),
  m_initTime(std::chrono::high_resolution_clock::now()),
  m_outstanding(std::move(outstanding))
{
  // Add filters from our factory. We're holding the lock for this factory, so use unsafe
  factory.AppendAutoFiltersToUnsafe(m_satCounters);
  
  // Traverse all descendant contexts, adding their packet subscriber vectors one at a time:
  for(const auto& curContext : ContextEnumerator(factory.GetContext()->FirstChild())) {
    AutowiredFast<AutoPacketFactory> curFactory(curContext);
    if(curFactory)
      // Only insert if this context actually has a packet factory
      curFactory->AppendAutoFiltersTo(m_satCounters);
  }

  // Sort, eliminate duplicates
  m_satCounters.sort();
  m_satCounters.erase(std::unique(m_satCounters.begin(), m_satCounters.end()), m_satCounters.end());

  // Prime the satisfaction graph for each element:
  for(auto& satCounter : m_satCounters)
    AddSatCounter(satCounter);

  // Initialize all counters:
  for (auto& satCounter : m_satCounters)
    satCounter.Reset();

  // Clear all references:
  for (auto& decoration : m_decorations)
    decoration.second.Reset();
}

AutoPacket::~AutoPacket(void) {
  m_parentFactory->RecordPacketDuration(
    std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::high_resolution_clock::now() - m_initTime
    )
  );
  
  // Needed for the AutoPacketGraph
  NotifyTeardownListeners();
}

DecorationDisposition& AutoPacket::CheckoutImmediateUnsafe(const std::type_info& ti, const void* pvImmed)
{
  // Obtain the decoration disposition of the entry we will be returning
  DecorationDisposition& dec = m_decorations[ti];

  // Ensure correct type if instantiated here
  dec.m_type = &ti;

  if (dec.satisfied || dec.isCheckedOut) {
    std::stringstream ss;
    ss << "Cannot perform immediate decoration with type " << autowiring::demangle(ti)
       << ", the requested decoration already exists";
    throw std::runtime_error(ss.str());
  }

  // Mark the entry as appropriate:
  dec.isCheckedOut = true;
  dec.satisfied = true;
  dec.m_pImmediate = pvImmed;
  return dec;
}

void AutoPacket::AddSatCounter(SatCounter& satCounter) {
  for(auto pCur = satCounter.GetAutoFilterInput(); *pCur; pCur++) {
    const std::type_info& dataType = *pCur->ti;
    DecorationDisposition* entry = &m_decorations[dataType];
    entry->m_type = &dataType;

    // Decide what to do with this entry:
    if (pCur->is_input) {
      entry->m_subscribers.push_back(&satCounter);
      if (entry->satisfied)
        satCounter.Decrement(dataType);
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
}

void AutoPacket::RemoveSatCounter(const SatCounter& satCounter) {
  for(auto pCur = satCounter.GetAutoFilterInput(); *pCur; pCur++) {
    const std::type_info& dataType = *pCur->ti;

    DecorationDisposition* entry = &m_decorations[dataType];

    // Decide what to do with this entry:
    if (pCur->is_input) {
      assert(!entry->m_subscribers.empty());
      assert(&satCounter == entry->m_subscribers.back());
      entry->m_subscribers.pop_back();
    }
    if (pCur->is_output) {
      assert(&satCounter == entry->m_publisher);
      entry->m_publisher = nullptr;
    }
  }
}

void AutoPacket::MarkUnsatisfiable(const std::type_info& info) {
  // TODO:
  // Add an anti-present decoration
}

void AutoPacket::UpdateSatisfaction(const std::type_info& info) {
  std::list<SatCounter*> callQueue;
  {
    std::lock_guard<std::mutex> lk(m_lock);

    auto dFind = m_decorations.find(info);
    if(dFind == m_decorations.end())
      // Trivial return, there's no subscriber to this decoration and so we have nothing to do
      return;

    // Update satisfaction inside of lock
    DecorationDisposition* decoration = &dFind->second;
    for(const auto& satCounter : decoration->m_subscribers)
      if(satCounter->Decrement(info))
        callQueue.push_back(satCounter);
  }

  // Make calls outside of lock, to avoid deadlock from decorations in methods
  for (SatCounter* call : callQueue)
    call->CallAutoFilter(*this);
}

void AutoPacket::PulseSatisfaction(DecorationDisposition* pTypeSubs[], size_t nInfos) {
  std::list<SatCounter*> callQueue;
  // First pass, decrement what we can:
  {
    std::lock_guard<std::mutex> lk(m_lock);
    for(size_t i = nInfos; i--;) {
      for(SatCounter*& cur : pTypeSubs[i]->m_subscribers) {
        if(
          // We only care about sat counters that aren't deferred--skip everyone else
          // Deferred calls will be too late.
          !cur->IsDeferred() &&

          // We cannot satisfy shared_ptr arguments, since the implied existence
          // guarantee of shared_ptr is violated
          !cur->GetArgumentType(pTypeSubs[i]->m_type)->is_shared &&

          // Now do the decrementation and proceed even if optional > 0,
          // since this is the only opportunity to fulfill the arguments
          (cur->Decrement(*pTypeSubs[i]->m_type) ||
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
    for(size_t i = nInfos; i--;)
      for(const auto& cur : pTypeSubs[i]->m_subscribers)
          cur->Increment(*pTypeSubs[i]->m_type);
  }
}

bool AutoPacket::UnsafeHas(const std::type_info& data) const {
  auto q = m_decorations.find(data);
  if(q == m_decorations.end())
    return false;
  return q->second.satisfied;
}

void AutoPacket::UnsafeCheckout(AnySharedPointer* ptr, const std::type_info& data) {
  auto& entry = m_decorations[data];
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

void AutoPacket::UnsafeComplete(bool ready, const std::type_info& ti, DecorationDisposition*& entry) {
  entry = &m_decorations[ti];

  assert(entry->m_type != nullptr); // CompleteCheckout must be for an initialized DecorationDisposition
  assert(entry->isCheckedOut); // CompleteCheckout must follow Checkout

  if(!ready)
    // Memory must be released, the checkout was cancelled
    entry->m_decoration->reset();

  // Reset the checkout flag before releasing the lock:
  entry->isCheckedOut = false;
  entry->satisfied = true;
}

void AutoPacket::CompleteCheckout(bool ready, const std::type_info& ti) {
  DecorationDisposition* entry = nullptr;
  {
    // This allows us to retrieve correct entries for decorated input requests
    std::lock_guard<std::mutex> guard(m_lock);
    UnsafeComplete(ready, ti, entry);
  }
  
  if (entry) {
    if (ready)
      UpdateSatisfaction(ti);
    else
      MarkUnsatisfiable(ti);
  }
}

const DecorationDisposition* AutoPacket::GetDisposition(const std::type_info& ti) const {
  std::lock_guard<std::mutex> lk(m_lock);

  auto q = m_decorations.find(ti);
  if (q != m_decorations.end() && q->second.satisfied)
    return &q->second;

  return nullptr;
}

bool AutoPacket::HasSubscribers(const std::type_info& ti) const {
  std::lock_guard<std::mutex> lk(m_lock);
  return m_decorations.count(ti) != 0;
}

const SatCounter& AutoPacket::GetSatisfaction(const std::type_info& subscriber) const {
  std::lock_guard<std::mutex> lk(m_lock);
  for (auto& sat : m_satCounters)
    if (sat.GetType() == &subscriber)
      return sat;
  throw autowiring_error("Attempted to get the satisfaction counter for an unavailable subscriber");
}

std::list<SatCounter> AutoPacket::GetSubscribers(const std::type_info& ti) const {
  std::lock_guard<std::mutex> lk(m_lock);
  std::list<SatCounter> subscribers;
  t_decorationMap::const_iterator decoration = m_decorations.find(ti);
  if (decoration != m_decorations.end())
    for (auto& subscriber : decoration->second.m_subscribers)
      subscribers.push_back(*subscriber);
  return subscribers;
}

std::list<DecorationDisposition> AutoPacket::GetDispositions(const std::type_info& ti) const {
  std::lock_guard<std::mutex> lk(m_lock);
  std::list<DecorationDisposition> dispositions;
  for (auto& disposition : m_decorations)
    if (disposition.second.m_type == &ti)
      dispositions.push_back(disposition.second);
  return dispositions;
}

void AutoPacket::Put(const std::type_info& ti, SharedPointerSlot&& in) {
  auto& entry = m_decorations[ti];
  if(entry.satisfied || entry.isCheckedOut) {
    std::stringstream ss;
    ss << "Cannot put type " << autowiring::demangle(in.type())
      << " on AutoPacket, the requested type already exists";
    throw std::runtime_error(ss.str());
  }

  entry.m_decoration = std::move(in);
  entry.m_type = &ti;
  entry.satisfied = true;
  entry.isCheckedOut = false;

  UpdateSatisfaction(ti);
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

      const std::type_info& data = *decoration.second.m_type;

      // Quietly drop data that is already present on recipient
      if (recipient->UnsafeHas(data))
        continue;

      AnySharedPointer any_ptr(decoration.second.m_decoration);
      recipient->UnsafeCheckout(&any_ptr, data);

      DecorationDisposition* entry = nullptr;
      recipient->UnsafeComplete(true, data, entry);
      decoQueue.push_back(entry);
    }

    m_lock.unlock();
    recipient->m_lock.unlock();
  }

  // Recipient satisfaction is updated outside of lock
  for (DecorationDisposition* broadDeco : decoQueue) {
    // Satisfy the base declaration first and then the shared pointer:
    recipient->UpdateSatisfaction(broadDeco->m_decoration->type());
  }
}

AutoPacket::Recipient AutoPacket::AddRecipient(const AutoFilterDescriptor& descriptor) {
  Recipient retVal;
  SatCounter* recipient;

  {
    std::lock_guard<std::mutex> lk(m_lock);

    // (1) Append & Initialize new satisfaction counter
    m_satCounters.push_front(descriptor);
    retVal.position = m_satCounters.begin();
    recipient = &m_satCounters.front();
    recipient->Reset();

    // (2) Update satisfaction & Append types from subscriber
    AddSatCounter(*recipient);

    // (3) Short-circuit if we don't need to call
    if(!*recipient)
      return retVal;
  }

  // (4) All types are satisfied, call AutoFilter now.
  recipient->CallAutoFilter(*this);
  return retVal;
}

void AutoPacket::RemoveRecipient(Recipient&& recipient) {
  // Copy out the iterator, and eliminate the iterator from the source structure
  auto q = recipient.position;
  recipient.position = m_satCounters.end();

  // Remove the recipient from our list
  std::lock_guard<std::mutex> lk(m_lock);
  RemoveSatCounter(*q);
  m_satCounters.erase(q);
}

std::list<DecorationDisposition> AutoPacket::GetDispositions() const {
  std::lock_guard<std::mutex> lk(m_lock);
  std::list<DecorationDisposition> dispositions;
  for (auto& disposition : m_decorations)
    dispositions.push_back(disposition.second);
  return dispositions;
}

std::shared_ptr<AutoPacket> AutoPacket::Successor(void) {
  std::lock_guard<std::mutex> lk(m_lock);
  
  // If successor doesn't already exists, create it
  if (!m_successor){
    m_successor = m_parentFactory->ConstructPacket();
  }
  
  return m_successor;
}
