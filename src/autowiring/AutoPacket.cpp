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
  m_outstanding(std::move(outstanding)),
  m_firstCounter(nullptr)
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

  // Safe linked list unwind
  for (auto cur = m_firstCounter; cur;) {
    auto next = cur->flink;
    delete cur;
    cur = next;
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
      for(SatCounter* cur : pTypeSubs[i]->m_subscribers)
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
  for (auto* sat = m_firstCounter; sat; sat = sat->flink)
    if (sat->GetType() == &subscriber)
      return *sat;
  throw autowiring_error("Attempted to get the satisfaction counter for an unavailable subscriber");
}

void AutoPacket::ThrowNotDecoratedException(const DecorationKey& key) {
  std::stringstream ss;
  ss << "Attempted to obtain a type " << autowiring::demangle(key.ti) << " which was not decorated on this packet";
  throw std::runtime_error(ss.str());
}

size_t AutoPacket::GetDecorationTypeCount(void) const
{
  std::lock_guard<std::mutex> lk(m_lock);
  return m_decorations.size();
}

AutoPacket::t_decorationMap AutoPacket::GetDecorations(void) const
{
  std::lock_guard<std::mutex> lk(m_lock);
  return m_decorations;
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

const SatCounter* AutoPacket::AddRecipient(const AutoFilterDescriptor& descriptor) {
  SatCounter& sat = *new SatCounter(descriptor);

  // Linked list insertion:
  {
    std::lock_guard<std::mutex> lk(m_lock);
    sat.flink = m_firstCounter;
    if (m_firstCounter)
      m_firstCounter->blink = &sat;
    m_firstCounter = &sat;

    // Update satisfaction & Append types from subscriber
    AddSatCounter(sat);
  }

  if (!sat.remaining)
    // Filter is ready to be called, oblige it
    sat.GetCall()(sat.GetAutoFilter(), *this);

  // Done
  return &sat;
}

void AutoPacket::RemoveRecipient(const SatCounter& recipient) {
  // Remove the recipient from our list
  std::lock_guard<std::mutex> lk(m_lock);
  if (recipient.blink)
    recipient.blink->flink = recipient.flink;
  if (recipient.flink)
    recipient.flink->blink = recipient.blink;
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

std::shared_ptr<CoreContext> AutoPacket::GetContext(void) const {
  return m_parentFactory->GetContext();
}

bool AutoPacket::Wait(std::condition_variable& cv, const AutoFilterDescriptorInput* inputs, std::chrono::nanoseconds duration) {
  auto stub = std::make_shared<SignalStub>(*this, cv);

  // This ad-hoc filter detects when all the requested decorations have been added, and then
  // sets the necessary flags to inform the user
  AddRecipient(
    AutoFilterDescriptor(
      stub,
      AutoFilterDescriptorStub(
        &typeid(AutoPacketFactory),
        inputs,
        false,
        [] (const AnySharedPointer& obj, AutoPacket&) {
          auto stub = obj->as<SignalStub>();

          // Completed, mark the output as satisfied and update the condition variable
          std::lock_guard<std::mutex>(stub->packet.m_lock);
          stub->is_satisfied = true;

          // Only notify while the condition variable is still valid
          if (stub->cv)
            stub->cv->notify_all();
        }
      )
    )
  );

  // This lambda will detect when the packet has been abandoned without all of the necessary
  // decorations.  In that case, the satisfaction flag is left in its initial state
  AddTeardownListener(
    [stub] {
      std::lock_guard<std::mutex>(stub->packet.m_lock);
      stub->is_complete = true;

      // Only notify the condition variable if it's still present
      if (stub->cv)
        stub->cv->notify_all();
    }
  );

  // Delay for the requested period:
  std::unique_lock<std::mutex> lk(m_lock);

  auto x = MakeAtExit([&stub] {
    // Clear the condition variable out of the stub to prevent anyone else from trying to signal it
    stub->cv = nullptr;
  });

  bool bRet =
      duration == std::chrono::nanoseconds::max() ?
      cv.wait(lk, [&] { return stub->is_complete; }), true :
      cv.wait_for(lk, duration, [&] { return stub->is_satisfied || stub->is_complete; });

  // If we didn't get all of the decorations we wanted we should throw an exception indicating this
  if (!stub->is_satisfied)
    throw autowiring_error("Not all of the requested decorations were available on a packet at the conclusion of Call");

  return bRet;
}
