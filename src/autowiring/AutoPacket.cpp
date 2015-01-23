// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoPacket.h"
#include "Autowired.h"
#include "AutoPacketFactory.h"
#include "AutoPacketInternal.hpp"
#include "AutoPacketProfiler.h"
#include "AutoFilterDescriptor.h"
#include "ContextEnumerator.h"
#include "SatCounter.h"
#include <algorithm>
#include <sstream>

using namespace autowiring;

AutoPacket::AutoPacket(AutoPacketFactory& factory, std::shared_ptr<void>&& outstanding):
  m_parentFactory(std::static_pointer_cast<AutoPacketFactory>(factory.shared_from_this())),
  m_outstanding(std::move(outstanding))
{}

AutoPacket::~AutoPacket(void) {
  m_parentFactory->RecordPacketDuration(
    std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::high_resolution_clock::now() - m_initTime
    )
  );
  
  // Mark decorations of successor packets that use decorations
  // originating from this packet as unsatisfiable
  for (auto& pair : m_decorations) {
    if (!pair.first.tshift &&
        pair.second.m_state != DispositionState::Satisfied) {
      MarkSuccessorsUnsatisfiable(DecorationKey(pair.first.ti, 0));
    }
  }

  // Needed for the AutoPacketGraph
  NotifyTeardownListeners();
  
  // Create vector of all successor packets that will be destroyed
  // This prevents recursive AutoPacket destructor calls
  std::vector<std::shared_ptr<AutoPacket>> packets;
  
  // Recurse through unique successors, storing them in our vector
  for (AutoPacket* current = this; current->m_successor.unique();) {
    packets.push_back(current->m_successor);
    
    // Reset and continue to next successor
    AutoPacket* prev_current = current;
    current = current->m_successor.get();
    prev_current->m_successor.reset();
  }
}

DecorationDisposition& AutoPacket::DecorateImmediateUnsafe(const DecorationKey& key, const void* pvImmed)
{
  // Obtain the decoration disposition of the entry we will be returning
  DecorationDisposition& dec = m_decorations[key];

  // Ensure correct key if instantiated here
  dec.SetKey(key);

  if (dec.m_state != DispositionState::Unsatisfied) {
    std::stringstream ss;
    ss << "Cannot perform immediate decoration with type " << autowiring::demangle(key.ti)
       << ", the requested decoration already exists";
    throw std::runtime_error(ss.str());
  }

  // Mark the entry as appropriate:
  dec.m_state = DispositionState::Satisfied;
  dec.m_pImmediate = pvImmed;
  return dec;
}

void AutoPacket::AddSatCounter(SatCounter& satCounter) {
  for(auto pCur = satCounter.GetAutoFilterInput(); *pCur; pCur++) {
    DecorationKey key(*pCur->ti, pCur->tshift);

    DecorationDisposition& entry = m_decorations[key];
    entry.SetKey(key);

    // Decide what to do with this entry:
    if (pCur->is_input) {
      if (entry.m_publishers.size() > 1 && !pCur->is_multi) {
        std::stringstream ss;
        ss << "Cannot add listener for multi-broadcast type " << autowiring::demangle(pCur->ti);
        throw std::runtime_error(ss.str());
      }

      entry.m_subscribers.push_back(&satCounter);
      if (entry.m_state == DispositionState::Satisfied)
        satCounter.Decrement();
    }
    if (pCur->is_output) {
      if(!entry.m_publishers.empty())
        for (SatCounter* subscriber : entry.m_subscribers)
          for(auto pOther = subscriber->GetAutoFilterInput(); *pOther; pOther++)
            if (!pOther->is_multi) {
              std::stringstream ss;
              ss << "Added identical data broadcasts of type " << autowiring::demangle(pCur->ti) << " with existing subscriber.";
              throw std::runtime_error(ss.str());
            }

      entry.m_publishers.push_back(&satCounter);
    }

    // Make sure decorations exist for timeshifts less that key's timeshift
    for (int tshift = 0; tshift < key.tshift; ++tshift) {
      DecorationKey shiftKey(key.ti, tshift);
      m_decorations[shiftKey].SetKey(shiftKey);
    }
  }
}

void AutoPacket::RemoveSatCounter(const SatCounter& satCounter) {
  for(auto pCur = satCounter.GetAutoFilterInput(); *pCur; pCur++) {
    DecorationKey key(*pCur->ti, pCur->tshift);

    DecorationDisposition* entry = &m_decorations[key];

    // Decide what to do with this entry:
    if (pCur->is_input) {
      assert(!entry->m_subscribers.empty());
      assert(&satCounter == entry->m_subscribers.back());
      entry->m_subscribers.pop_back();
    }
    if (pCur->is_output) {
      assert(!entry->m_publishers.empty());
      assert(&satCounter == entry->m_publishers.back());
      entry->m_publishers.pop_back();
    }
  }
}

void AutoPacket::MarkUnsatisfiable(const DecorationKey& key) {
  // Perform unsatisfaction logic
  if (key.tshift) {
    {
      std::lock_guard<std::mutex> lk(m_lock);
      auto& disp = m_decorations[key];
      disp.m_state = DispositionState::Satisfied;
      disp.m_decorations.clear();
    }
    UpdateSatisfaction(key);
  }
}

void AutoPacket::MarkSuccessorsUnsatisfiable(DecorationKey key) {
  std::lock_guard<std::mutex> lk(m_lock);
  
  // Update key and successor
  key.tshift++;
  auto successor = SuccessorUnsafe();
  
  while (m_decorations.count(key)) {
    successor->MarkUnsatisfiable(key);
    
    // Update key and successor
    key.tshift++;
    successor = successor->Successor();
  }
}

void AutoPacket::UpdateSatisfaction(const DecorationKey& info) {
  std::vector<SatCounter*> callQueue;
  {
    std::lock_guard<std::mutex> lk(m_lock);

    auto dFind = m_decorations.find(info);
    if(dFind == m_decorations.end())
      // Trivial return, there's no subscriber to this decoration and so we have nothing to do
      return;

    // Update satisfaction inside of lock
    DecorationDisposition& decoration = dFind->second;
    for(SatCounter* satCounter : decoration.m_subscribers)
      if(satCounter->Decrement())
        callQueue.push_back(satCounter);
  }

  // Make calls outside of lock, to avoid deadlock from decorations in methods
  for (SatCounter* call : callQueue)
    call->GetCall()(call->GetAutoFilter(), *this);
}

void AutoPacket::PulseSatisfaction(DecorationDisposition* pTypeSubs[], size_t nInfos) {
  std::vector<SatCounter*> callQueue;

  // First pass, decrement what we can:
  {
    std::lock_guard<std::mutex> lk(m_lock);
    for(size_t i = nInfos; i--;) {
      for(SatCounter* cur : pTypeSubs[i]->m_subscribers) {
        if(
          // We only care about sat counters that aren't deferred--skip everyone else
          // Deferred calls will be too late.
          !cur->IsDeferred() &&

          // We cannot satisfy shared_ptr arguments, since the implied existence
          // guarantee of shared_ptr is violated
          !cur->GetArgumentType(&pTypeSubs[i]->GetKey().ti)->is_shared &&

          // Now do the decrementation and proceed even if optional > 0,
          // since this is the only opportunity to fulfill the arguments
          cur->Decrement()
        )
          // Finally, queue a call for this type
          callQueue.push_back(cur);
      }
    }
  }

  // Make calls outside of lock, to avoid deadlock from decorations in methods
  for (SatCounter* call : callQueue)
    call->GetCall()(call->GetAutoFilter(), *this);

  // Reset all counters, data in this call will not be available on return
  {
    std::lock_guard<std::mutex> lk(m_lock);
    for(size_t i = nInfos; i--;)
      for(const auto& cur : pTypeSubs[i]->m_subscribers)
        cur->Increment();
  }
}

bool AutoPacket::HasUnsafe(const DecorationKey& key) const {
  auto q = m_decorations.find(key);
  if(q == m_decorations.end())
    return false;
  return q->second.m_state == DispositionState::Satisfied;
}

void AutoPacket::DecorateUnsafeNoPriors(const AnySharedPointer& ptr, const DecorationKey& key) {
  auto& entry = m_decorations[key];
  entry.SetKey(key); // Ensure correct key if instantiated here

  if (entry.m_state == DispositionState::Satisfied) {
    std::stringstream ss;
    ss << "Cannot decorate this packet with type " << autowiring::demangle(ptr)
      << ", the requested decoration is already satisfied";
    throw std::runtime_error(ss.str());
  }
  if (entry.m_state == DispositionState::Unsatisfiable) {
    std::stringstream ss;
    ss << "Cannot check out decoration of type " << autowiring::demangle(ptr)
      << ", it has been marked unsatisfiable";
    throw std::runtime_error(ss.str());
  }

  entry.m_decorations.push_back(ptr);

  if (entry.m_decorations.size() >= entry.m_publishers.size()) {
    entry.m_state = DispositionState::Satisfied;
  } else {
    entry.m_state = DispositionState::PartlySatisfied;
  }

  // This allows us to retrieve correct entries for decorated input requests
  AnySharedPointer decoration(entry.m_decorations.back());
}

void AutoPacket::Decorate(const AnySharedPointer& ptr, DecorationKey key) {
  auto cur = shared_from_this();

  do {
    // Decorate, first, while holding down a lock
    (std::lock_guard<std::mutex>)cur->m_lock,
    cur->DecorateUnsafeNoPriors(ptr, key);

    // Now update satisfaction set on this entry
    if (m_decorations[key].m_state == DispositionState::Satisfied) {
      cur->UpdateSatisfaction(key);
    }

    // Obtain the successor
    cur = cur->Successor();
    key.tshift++;
  } while (
    // If there are any filters on _this_ packet that desire to know the prior packet, then
    // we must proactively preserve the value of this decoration for our successor.
    (std::lock_guard<std::mutex>)m_lock,
    m_decorations.count(key)
  );
}

const DecorationDisposition* AutoPacket::GetDisposition(const DecorationKey& key) const {
  std::lock_guard<std::mutex> lk(m_lock);

  auto q = m_decorations.find(key);
  if (q != m_decorations.end() && q->second.m_state == DispositionState::Satisfied)
    return &q->second;

  return nullptr;
}

bool AutoPacket::HasSubscribers(const DecorationKey& key) const {
  std::lock_guard<std::mutex> lk(m_lock);
  return m_decorations.count(key) != 0;
}

const SatCounter& AutoPacket::GetSatisfaction(const std::type_info& subscriber) const {
  std::lock_guard<std::mutex> lk(m_lock);
  for (auto& sat : m_satCounters)
    if (sat.GetType() == &subscriber)
      return sat;
  throw autowiring_error("Attempted to get the satisfaction counter for an unavailable subscriber");
}

std::list<SatCounter> AutoPacket::GetSubscribers(const DecorationKey& key) const {
  std::lock_guard<std::mutex> lk(m_lock);
  std::list<SatCounter> subscribers;
  t_decorationMap::const_iterator decoration = m_decorations.find(key);
  if (decoration != m_decorations.end())
    for (auto& subscriber : decoration->second.m_subscribers)
      subscribers.push_back(*subscriber);
  return subscribers;
}

std::list<DecorationDisposition> AutoPacket::GetDispositions(const DecorationKey& key) const {
  std::lock_guard<std::mutex> lk(m_lock);
  std::list<DecorationDisposition> dispositions;
  for (auto& disposition : m_decorations)
    if (disposition.second.GetKey() == key)
      dispositions.push_back(disposition.second);
  return dispositions;
}

void AutoPacket::ThrowNotDecoratedException(const DecorationKey& key) {
  std::stringstream ss;
  ss << "Attempted to obtain a type " << autowiring::demangle(key.ti) << " which was not decorated on this packet";
  throw std::runtime_error(ss.str());
}

void AutoPacket::ForwardAll(std::shared_ptr<AutoPacket> recipient) const {
  // Copy decorations into an internal decorations maintenance collection.  The values
  // in this collection are guaranteed to be stable in memory, and there are stable states
  // that can be relied upon without synchronization.
  std::vector<std::pair<const DecorationDisposition*, AnySharedPointer>> dd;
  {
    std::lock_guard<std::mutex> lk(m_lock);
    for (const auto& decoration : m_decorations)
      // Only fully satisfied decorations are considered for propagation
      if (decoration.second.m_state == DispositionState::Satisfied)
        for (const auto& single : decoration.second.m_decorations)
          dd.push_back(std::make_pair(&decoration.second, single));
  }

  // Lock down recipient colleciton while we go through and attach decorations:
  for (auto& cur : dd) {
    {
      std::lock_guard<std::mutex> lk(recipient->m_lock);
      if (recipient->HasUnsafe(cur.first->GetKey()))
        // Key already present, circle around
        continue;
    }

    // Decorate while unsynchronized:
    recipient->Decorate(cur.second, cur.first->GetKey());
  }
}

AutoPacket::Recipient AutoPacket::AddRecipient(const AutoFilterDescriptor& descriptor) {
  Recipient retVal;
  SatCounter* recipient;

  {
    std::lock_guard<std::mutex> lk(m_lock);

    // Append & Initialize new satisfaction counter
    m_satCounters.push_front(descriptor);
    retVal.position = m_satCounters.begin();
    recipient = &m_satCounters.front();

    // Update satisfaction & Append types from subscriber
    AddSatCounter(*recipient);

    // Short-circuit if we don't need to call
    if(recipient->remaining)
      return retVal;
  }

  // Filter is ready to be called, oblige it
  recipient->GetCall()(recipient->GetAutoFilter(), *this);
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
  return SuccessorUnsafe();
}

std::shared_ptr<AutoPacket> AutoPacket::SuccessorUnsafe(void) {
  // If successor doesn't already exists, create it
  if (!m_successor){
    m_successor = m_parentFactory->ConstructPacket();
  }

  return m_successor;
}

