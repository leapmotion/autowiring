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
  for (auto& pair : m_decorations)
    if (!pair.first.tshift && pair.second.m_state != DispositionState::Satisfied)
      MarkSuccessorsUnsatisfiable(DecorationKey(*pair.first.ti, pair.first.is_shared, 0));

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
    DecorationKey key(*pCur->ti, pCur->is_shared, pCur->tshift);
    DecorationDisposition& entry = m_decorations[key];

    // Decide what to do with this entry:
    if (pCur->is_input) {
      if (entry.m_publishers.size() > 1 && !pCur->is_multi) {
        std::stringstream ss;
        ss << "Cannot add listener for multi-broadcast type " << autowiring::demangle(pCur->ti);
        throw std::runtime_error(ss.str());
      }

      entry.m_subscribers.push_back(&satCounter);
      switch (entry.m_state) {
      case DispositionState::Satisfied:
      case DispositionState::Unsatisfiable:
        satCounter.Decrement();
        break;
      }
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
    for (int tshift = 0; tshift < key.tshift; ++tshift)
      m_decorations[DecorationKey(*key.ti, key.is_shared, tshift)];
  }
}

void AutoPacket::MarkUnsatisfiable(const DecorationKey& key) {
  // Ensure correct type if instantiated here
  std::unique_lock<std::mutex> lk(m_lock);
  auto& entry = m_decorations[key];

  if (entry.m_state == DispositionState::PartlySatisfied || entry.m_state == DispositionState::Satisfied)
    throw std::runtime_error("Cannot mark a decoration as unsatisfiable when that decoration is already present on this packet");

  // Mark the entry as permanently unsatisfiable:
  entry.m_state =
    key.is_shared ?
    DispositionState::Unsatisfiable :
    DispositionState::UnsatisfiableNoCall;

  // Notify all consumers
  UpdateSatisfactionUnsafe(std::move(lk), entry);
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

void AutoPacket::UpdateSatisfactionUnsafe(std::unique_lock<std::mutex>&& lk, const DecorationDisposition& disposition) {
  std::vector<SatCounter*> callQueue;

  // Update satisfaction inside of lock
  switch (disposition.m_state) {
  case DispositionState::Unsatisfiable:
  case DispositionState::Satisfied:
    for (SatCounter* satCounter : disposition.m_subscribers)
      if (satCounter->Decrement())
        callQueue.push_back(satCounter);
    break;
  default:
    // Nothing to do
    return;
  }

  // Make calls outside of lock, to avoid deadlock from decorations in methods
  lk.unlock();
  for (SatCounter* call : callQueue)
    call->GetCall()(call->GetAutoFilter(), *this);
}

void AutoPacket::PulseSatisfaction(DecorationDisposition* pTypeSubs[], size_t nInfos) {
  std::vector<SatCounter*> callQueue;

  // First pass, decrement what we can:
  {
    std::lock_guard<std::mutex> lk(m_lock);
    for(size_t i = nInfos; i--;)
      for(SatCounter* cur : pTypeSubs[i]->m_subscribers) {
        if(
          // We only care about sat counters that aren't deferred--skip everyone else
          // Deferred calls will be too late.
          !cur->IsDeferred() &&

          // Now do the decrementation and proceed even if optional > 0,
          // since this is the only opportunity to fulfill the arguments
          cur->Decrement()
        )
          // Finally, queue a call for this type
          callQueue.push_back(cur);
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

void AutoPacket::DecorateNoPriors(const AnySharedPointer& ptr, DecorationKey key) {
  DecorationDisposition* dispositionA;
  DecorationDisposition* dispositionB;
  {
    std::lock_guard<std::mutex> lk(m_lock);

    auto transition = [&](DecorationKey& key){
      DecorationDisposition& disposition = m_decorations[key];
      switch (disposition.m_state) {
      case DispositionState::Satisfied:
        {
          std::stringstream ss;
          ss << "Cannot decorate this packet with type " << autowiring::demangle(ptr)
            << ", the requested decoration is already satisfied";
          throw std::runtime_error(ss.str());
        }
        break;
      case DispositionState::Unsatisfiable:
      case DispositionState::UnsatisfiableNoCall:
        {
          std::stringstream ss;
          ss << "Cannot check out decoration of type " << autowiring::demangle(ptr)
            << ", it has been marked unsatisfiable";
          throw std::runtime_error(ss.str());
        }
        break;
      }

      // Decoration attaches here
      disposition.m_decorations.push_back(ptr);
      return &disposition;
    };

    key.is_shared = false;
    dispositionA = transition(key);
    key.is_shared = true;
    dispositionB = transition(key);
  }

  // Uniformly advance state:
  switch (dispositionA->m_state) {
  case DispositionState::Unsatisfied:
  case DispositionState::PartlySatisfied:
    // Permit a transition to another state
    if (dispositionA->IsPublicationComplete() && dispositionB->IsPublicationComplete()) {
      dispositionA->m_state = dispositionB->m_state = DispositionState::Satisfied;

      UpdateSatisfactionUnsafe(std::unique_lock<std::mutex>(m_lock), *dispositionA);
      UpdateSatisfactionUnsafe(std::unique_lock<std::mutex>(m_lock), *dispositionB);
    }
    else
      dispositionA->m_state = dispositionB->m_state = DispositionState::PartlySatisfied;
    break;
  default:
    // Do nothing, no advancing to any states from here
    break;
  }
}

void AutoPacket::Decorate(const AnySharedPointer& ptr, DecorationKey key) {
  auto cur = shared_from_this();

  do {
    // Update satisfaction set on this entry
    cur->DecorateNoPriors(ptr, key);

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
  std::vector<t_decorationMap::value_type> dd;
  {
    std::lock_guard<std::mutex> lk(m_lock);
    for (const auto& decoration : m_decorations)
      // Only fully satisfied shared decorations are considered for propagation
      if (
        decoration.first.is_shared &&
        decoration.second.m_state == DispositionState::Satisfied
      )
        dd.push_back(decoration);
  }

  // Lock down recipient collection while we go through and attach decorations:
  for (auto& cur : dd)
    for (const auto& decoration : cur.second.m_decorations)
      recipient->Decorate(decoration, cur.first);
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
