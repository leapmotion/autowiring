// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoCurrentPacketPusher.h"
#include "AutoPacket.h"
#include "AutoPacketFactory.h"
#include "AutoPacketInternal.hpp"
#include "AutoFilterDescriptor.h"
#include "autowiring_error.h"
#include "ContextEnumerator.h"
#include "demangle.h"
#include "SatCounter.h"
#include "thread_specific_ptr.h"
#include <algorithm>
#include <sstream>
#include RVALUE_HEADER

using namespace autowiring;

/// <summary>
/// A pointer to the current AutoPacket, specific to the current thread.
/// </summary>
static thread_specific_ptr<AutoPacket> autoCurrentPacket{ nullptr };

AutoPacket::AutoPacket(AutoPacketFactory& factory, std::shared_ptr<void>&& outstanding):
  m_parentFactory(std::static_pointer_cast<AutoPacketFactory>(factory.shared_from_this())),
  m_outstanding(std::move(outstanding))
{
  // Need to ensure our identity type is instantiated
  (void) auto_id_t_init<AutoPacket>::init;
}

AutoPacket::~AutoPacket(void) {
  m_parentFactory->RecordPacketDuration(
    std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::high_resolution_clock::now() - m_initTime
    )
  );
  
  // Mark decorations of successor packets that use decorations
  // originating from this packet as unsatisfiable
  for (auto& pair : m_decoration_map)
    if (!pair.first.tshift && pair.second.m_state != DispositionState::Complete)
      MarkSuccessorsUnsatisfiable(DecorationKey(pair.first.id, 0));

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
  DecorationDisposition& dec = m_decoration_map[key];

  if (dec.m_state != DispositionState::Unsatisfied) {
    std::stringstream ss;
    ss << "Cannot perform immediate decoration with type " << autowiring::demangle(key.id)
       << ", the requested decoration already exists";
    throw autowiring_error(ss.str());
  }

  // Mark the entry as appropriate:
  dec.m_state = DispositionState::Complete;
  dec.m_pImmediate = pvImmed;
  return dec;
}

void AutoPacket::AddSatCounterUnsafe(SatCounter& satCounter) {
  for(auto pCur = satCounter.GetAutoFilterArguments(); *pCur; pCur++) {
    DecorationKey key(pCur->id, pCur->tshift);
    DecorationDisposition& entry = m_decoration_map[key];

    // Make sure decorations exist for timeshifts less that key's timeshift
    for (int tshift = 0; tshift < key.tshift; ++tshift)
      m_decoration_map[DecorationKey(key.id, tshift)];

    // Decide what to do with this entry:
    if (pCur->is_input) {
      if (entry.m_publishers.size() > 1 && !pCur->is_multi) {
        std::stringstream ss;
        ss << "Cannot add listener for multi-broadcast type " << autowiring::demangle(pCur->id);
        throw autowiring_error(ss.str());
      }
      if (entry.m_state == DispositionState::Complete) {
        // Either decorations must be present, or the decoration type must be a shared_ptr.
        if (!entry.m_decorations.empty() || pCur->is_shared) {
          satCounter.Decrement();
        }
      }
    }

    if (pCur->is_rvalue) {
      // Throw exception when there is already a modifier with the same altitude,
      // otherwise insert it to the right position so that the modifiers vector is sorted by altitude
      auto it = entry.m_modifiers.begin();
      while (it != entry.m_modifiers.end()) {
        if (*it == nullptr)
          continue;

        if ((*it)->GetAltitude() == satCounter.GetAltitude()) {
          std::stringstream ss;
          ss << "Added multiple rvalue decorations with same altitudes for type " << autowiring::demangle(pCur->id);
          throw autowiring_error(ss.str());
        }

        if ((*it)->GetAltitude() < satCounter.GetAltitude())
          break;
        it++;
      }
      entry.m_modifiers.insert(it, &satCounter);
    } else {
      if (pCur->is_input) {
          entry.m_subscribers.emplace(
          pCur->is_shared,
          pCur->is_multi ?
          DecorationDisposition::Subscriber::Type::Multi :
          pCur->is_shared ?
          DecorationDisposition::Subscriber::Type::Optional :
          DecorationDisposition::Subscriber::Type::Normal,
          satCounter.GetAltitude(),
          &satCounter
        );
      }

      if (pCur->is_output) {
        if (!entry.m_publishers.empty())
          for (const auto& subscriber : entry.m_subscribers)
            for (auto pOther = subscriber.satCounter->GetAutoFilterArguments(); *pOther; pOther++) {
              if (pOther->id == pCur->id && !pOther->is_multi) {
                std::stringstream ss;
                ss << "Added identical data broadcasts of type " << autowiring::demangle(pCur->id) << " with existing subscriber.";
                throw autowiring_error(ss.str());
              }
            }
        entry.m_publishers.push_back(&satCounter);
      }
    }
  }

  auto tempVisited = std::unordered_set<SatCounter*>();
  auto permVisited = std::unordered_set<SatCounter*>();
  DetectCycle(satCounter, tempVisited, permVisited);
}

void AutoPacket::DetectCycle(SatCounter& satCounter, std::unordered_set<SatCounter*>& tempVisited, std::unordered_set<SatCounter*>& permVisited) {
  if (tempVisited.count(&satCounter)) {
    std::stringstream ss;
    ss << "Detected cycle in the auto filter graph involving type " << autowiring::demangle(satCounter.GetType());
    throw autowiring_error(ss.str());
  }

  if (permVisited.count(&satCounter))
    return;

  std::unordered_set<SatCounter*> nextCounters;
  for(auto pCur = satCounter.GetAutoFilterArguments(); *pCur; pCur++) {
    if (!pCur->is_output) continue;

    DecorationKey key(pCur->id, pCur->tshift);
    DecorationDisposition& entry = m_decoration_map[key];
    for (auto& subscriber : entry.m_subscribers) {
      auto ptr = subscriber.satCounter;
      nextCounters.insert(ptr);
    }
  }
  if (nextCounters.empty())
    return;

  tempVisited.insert(&satCounter);
  for (auto pCounter : nextCounters) {
    DetectCycle(*pCounter, tempVisited, permVisited);
  }
  permVisited.insert(&satCounter);
  tempVisited.erase(&satCounter);
}

void AutoPacket::RemoveSatCounterUnsafe(const SatCounter& satCounter) {
  for (auto pCur = satCounter.GetAutoFilterArguments(); *pCur; pCur++) {
    DecorationKey key(pCur->id, pCur->tshift);
    DecorationDisposition& entry = m_decoration_map[key];

    if (pCur->is_rvalue) {
      entry.m_modifiers.erase(
        std::remove_if(
          entry.m_modifiers.begin(),
          entry.m_modifiers.end(),
          [&satCounter](SatCounter* modifier){
            return modifier && *modifier == satCounter;
          }),
        entry.m_modifiers.end()
      );
    } else {
      if (pCur->is_input) {
        for (auto& sub : entry.m_subscribers) {
          if (sub.satCounter && *sub.satCounter == satCounter) {
            entry.m_subscribers.erase(sub);
            break;
          }
        }
      }
      if (pCur->is_output) {
        entry.m_publishers.erase(
          std::remove_if(
            entry.m_publishers.begin(),
            entry.m_publishers.end(),
            [&satCounter](SatCounter* publisher){
              return publisher && *publisher == satCounter;
            }),
          entry.m_publishers.end()
        );
      }
    }
  }
}

void AutoPacket::MarkUnsatisfiable(const DecorationKey& key) {
  // Ensure correct type if instantiated here
  std::unique_lock<std::mutex> lk(m_lock);
  auto& entry = m_decoration_map[key];

  // Clear all decorations and pointers attached here
  entry.m_state = DispositionState::Complete;
  entry.m_decorations.clear();
  entry.m_pImmediate = nullptr;

  // Notify all consumers
  UpdateSatisfactionUnsafe(std::move(lk), entry);
}

void AutoPacket::MarkSuccessorsUnsatisfiable(DecorationKey key) {
  std::lock_guard<std::mutex> lk(m_lock);
  
  // Update key and successor
  key.tshift++;
  auto successor = SuccessorUnsafe();
  
  while (m_decoration_map.count(key)) {
    successor->MarkUnsatisfiable(key);
    
    // Update key and successor
    key.tshift++;
    successor = successor->Successor();
  }
}

void AutoPacket::UpdateSatisfactionUnsafe(std::unique_lock<std::mutex> lk, const DecorationDisposition& disposition) {
  // Update satisfaction inside of lock
  if (disposition.m_state != DispositionState::Complete)
    // Nothing to do yet
    return;

  // Any filter that who can take this decoration as an optional input should be called
  std::vector<SatCounter*> callQueue;

  // Recursively mark unsatisfiable any single-output arguments on these subscribers:
  std::vector<const AutoFilterArgument*> unsatOutputArgs;

  switch (disposition.m_decorations.size()) {
  case 0:
    // No decorations here whatsoever.
    // Subscribers that cannot be invoked should have their outputs recursively marked unsatisfiable.
    // Subscribers that can be invoked should be.
    for (auto subscriber : disposition.m_subscribers) {
      auto& satCounter = *subscriber.satCounter;
      if (!satCounter.remaining)
        // Skip subscribers that have already been called--a decoration is being expunged from the packet,
        // but the filter in question has already been invoked, and so its outputs are already on the packet
        continue;

      switch (subscriber.type) {
      case DecorationDisposition::Subscriber::Type::Multi:
      case DecorationDisposition::Subscriber::Type::Optional:
        // Optional, we will just generate a call to this subscriber, if possible:
        if (satCounter.Decrement())
          callQueue.push_back(&satCounter);
        break;
      case DecorationDisposition::Subscriber::Type::Normal:
        {
          // Non-optional, consider outputs and recursively invalidate
          const auto* args = satCounter.GetAutoFilterArguments();
          for (size_t i = satCounter.GetArity(); i--;) {
            // Only consider output arguments:
            if (args[i].is_output)
              // This output is transitively unsatisfiable, include it for later removal
              unsatOutputArgs.push_back(&args[i]);
          }
        }
        break;
      }
    }
    break;
  case 1:
    {
      // One unique decoration available.  We should be able to call everyone.
      for (auto pMod : disposition.m_modifiers) {
        if (pMod && pMod->Decrement()) {
          callQueue.push_back(pMod);
        }
      }
      for (auto subscriber : disposition.m_subscribers) {
        auto& satCounter = *subscriber.satCounter;
        if (satCounter.Decrement())
          callQueue.push_back(&satCounter);
      }
    }
    break;
  default:
    // Multiple decorations.  Single-input types should never be encountered, but if they are,
    // we can't call them.  Always call multi-input entries.
    for (auto subscriber : disposition.m_subscribers) {
      if (subscriber.type != DecorationDisposition::Subscriber::Type::Multi)
        throw autowiring_error("An AutoFilter was detected which has single-decorate inputs in a graph with multi-decorate outputs");

      // One more entry for this input to consider
      if(subscriber.satCounter->Decrement())
        callQueue.push_back(subscriber.satCounter);
    }
    break;
  }
  lk.unlock();

  // Generate all calls
  {
    AutoCurrentPacketPusher apkt(*this);
    for (SatCounter* call : callQueue)
      call->GetCall()(call->GetAutoFilter().ptr(), *this);
  }

  // Mark all unsatisfiable output types
  for (auto unsatOutputArg : unsatOutputArgs) {
    // One more producer run, even though we couldn't attach any new decorations
    auto& entry = m_decoration_map[DecorationKey{unsatOutputArg->id, 0}];
    entry.m_nProducersRun++;

    // Now recurse on this entry
    UpdateSatisfactionUnsafe(std::unique_lock<std::mutex>{m_lock}, entry);
  }
}

void AutoPacket::PulseSatisfactionUnsafe(std::unique_lock<std::mutex> lk, DecorationDisposition* pTypeSubs[], size_t nInfos) {
  std::vector<SatCounter*> callQueue;
  std::vector<SatCounter*> reincrement;

  // We will need to loop as long as each pass results in something more to be executed
  do
  {
    // Empty everything first
    callQueue.clear();
    reincrement.clear();

    // First pass, decrement what we can:
    for (size_t i = nInfos; i--;)
      for (const auto& cur : pTypeSubs[i]->m_subscribers) {
        SatCounter* satCounter = cur.satCounter;
        if (
          // Require that this counter not need a shared pointer, because we can't provide one
          !cur.is_shared &&

          // We only care about sat counters that aren't deferred--skip everyone else
          // Deferred calls will be too late.
          !satCounter->IsDeferred()
        )
        {
          if (satCounter->Decrement())
            // This one is satisfied, we will need to call it
            callQueue.push_back(satCounter);
          else
            // Reincrement.  We need to retain total control over this immediate-mode decoration.
            // If a decrementation happens in another thread context due to a simultaneous call
            // to DecorateImmediate, we could wind up with a race condition.  Multiple decorations
            // passed to DecorateImmediate may satisfy simultaneously, however, so we want to
            // delay reincrementation until after the loop concludes
            reincrement.push_back(satCounter);
        }
      }

    // Reincrement anything that didn't zero out.  This is safe to do even if some of these
    // entries are present in the callQueue; if there's a call in the callQueue, we will assign
    // the saturation counter to zero anyway.
    for (auto& cur : reincrement)
      cur->Increment();

    // Run through calls while unsynchronized:
    lk.unlock();
    for (SatCounter* call : callQueue) {
      call->GetCall()(call->GetAutoFilter().ptr(), *this);
      call->remaining = 0;
    }
    lk.lock();
  } while (!callQueue.empty());
}

bool AutoPacket::HasUnsafe(const DecorationKey& key) const {
  auto q = m_decoration_map.find(key);
  if(q == m_decoration_map.end())
    return false;
  return !q->second.m_decorations.empty();
}

void AutoPacket::DecorateNoPriors(const AnySharedPointer& ptr, DecorationKey key) {
  DecorationDisposition* disposition;
  std::unique_lock<std::mutex> lk(m_lock);

  disposition = &m_decoration_map[key];
  switch (disposition->m_state) {
  case DispositionState::Complete:
    {
      std::stringstream ss;
      if (disposition->m_decorations.empty())
        // Completed with no decorations, unsatisfiable
        ss << "Cannot check out decoration of type " << autowiring::demangle(ptr)
          << ", it has been marked unsatisfiable";
      else
        ss << "Cannot decorate this packet with type " << autowiring::demangle(ptr)
          << ", the requested decoration is already satisfied";
      throw autowiring_error(ss.str());
    }
    break;
  default:
    break;
  }

  // Decoration attaches here
  disposition->m_decorations.push_back(ptr);
  disposition->m_nProducersRun++;

  // Uniformly advance state:
  switch (disposition->m_state) {
  case DispositionState::Unsatisfied:
  case DispositionState::PartlySatisfied:
    // Permit a transition to another state
    if (disposition->IsPublicationComplete()) {
      disposition->m_state = DispositionState::Complete;
      UpdateSatisfactionUnsafe(std::move(lk), *disposition);
    }
    else
      disposition->m_state = DispositionState::PartlySatisfied;
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
    m_decoration_map.count(key)
  );
}

const DecorationDisposition* AutoPacket::GetDisposition(const DecorationKey& key) const {
  std::lock_guard<std::mutex> lk(m_lock);

  auto q = m_decoration_map.find(key);
  if (q != m_decoration_map.end() && q->second.m_state == DispositionState::Complete)
    return &q->second;

  return nullptr;
}

bool AutoPacket::HasSubscribers(const DecorationKey& key) const {
  std::lock_guard<std::mutex> lk(m_lock);
  auto q = m_decoration_map.find(key);
  return
    q == m_decoration_map.end() ?
    false :
    q->second.m_subscribers.size() != 0;
}

size_t AutoPacket::HasPublishers(const DecorationKey& key) const {
  std::lock_guard<std::mutex> lk(m_lock);
  auto q = m_decoration_map.find(key);
  return
    q == m_decoration_map.end() ?
    0 :
    q->second.m_publishers.size();
}

const SatCounter& AutoPacket::GetSatisfaction(auto_id subscriber) const {
  std::lock_guard<std::mutex> lk(m_lock);
  for (auto* sat = m_firstCounter; sat; sat = sat->flink)
    if (sat->GetType() == subscriber)
      return *sat;
  throw autowiring_error("Attempted to get the satisfaction counter for an unavailable subscriber");
}

void AutoPacket::ThrowNotDecoratedException(const DecorationKey& key) {
  std::stringstream ss;
  ss << "Attempted to obtain a type " << autowiring::demangle(key.id) << " which was not decorated on this packet";
  throw autowiring_error(ss.str());
}

void AutoPacket::ThrowMultiplyDecoratedException(const DecorationKey& key) {
  std::stringstream ss;
  ss << "Attempted to obtain a type " << autowiring::demangle(key.id) << " which was decorated more than once on this packet";
  throw autowiring_error(ss.str());
}

size_t AutoPacket::GetDecorationTypeCount(void) const
{
  std::lock_guard<std::mutex> lk(m_lock);
  return m_decoration_map.size();
}

AutoPacket::t_decorationMap AutoPacket::GetDecorations(void) const
{
  std::lock_guard<std::mutex> lk(m_lock);
  return m_decoration_map;
}

bool AutoPacket::IsUnsatisfiable(const auto_id& id) const
{
  const DecorationDisposition* pDisposition = GetDisposition(DecorationKey{ id, 0 });
  if (!pDisposition)
    return false;
  if (!pDisposition->m_decorations.empty())
    return false;
  if (pDisposition->m_nProducersRun == pDisposition->m_publishers.size())
    return false;
  return true;
}

void AutoPacket::ForwardAll(const std::shared_ptr<AutoPacket>& recipient) const {
  // Copy decorations into an internal decorations maintenance collection.  The values
  // in this collection are guaranteed to be stable in memory, and there are stable states
  // that can be relied upon without synchronization.
  std::vector<std::pair<DecorationKey, DecorationDisposition>> dd;
  {
    std::lock_guard<std::mutex> lk(m_lock);
    for (const auto& decoration : m_decoration_map)
      // Only fully complete decorations are considered for propagation
      if (decoration.second.m_state == DispositionState::Complete)
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
    for (auto cur = m_firstCounter; cur; cur=cur->flink) {
      if (*cur == sat)
        return cur;
    }

    sat.flink = m_firstCounter;
    if (m_firstCounter)
      m_firstCounter->blink = &sat;
    m_firstCounter = &sat;

    // Update satisfaction & Append types from subscriber
    AddSatCounterUnsafe(sat);
  }

  if (!sat.remaining)
    // Filter is ready to be called, oblige it
    sat.GetCall()(sat.GetAutoFilter().ptr(), *this);

  return &sat;
}

void AutoPacket::RemoveRecipient(const SatCounter& recipient) {
  // Remove the recipient from our list
  std::lock_guard<std::mutex> lk(m_lock);
  if (recipient.blink)
    recipient.blink->flink = recipient.flink;
  if (recipient.flink)
    recipient.flink->blink = recipient.blink;

  RemoveSatCounterUnsafe(recipient);
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

AutoPacket* AutoPacket::SetCurrent(AutoPacket* apkt) {
  AutoPacket* prior = autoCurrentPacket.get();
  if(apkt)
    autoCurrentPacket.reset(apkt);
  else
    autoCurrentPacket.release();
  return prior;
}

AutoPacket& AutoPacket::CurrentPacket(void) {
  auto retVal = autoCurrentPacket.get();
  if (!retVal)
    throw autowiring_error("Attempted to obtain a current AutoPacket, which was not made");

  return *retVal;
}

std::shared_ptr<CoreContext> AutoPacket::GetContext(void) const {
  return m_parentFactory->GetContext();
}

bool AutoPacket::Wait(std::condition_variable& cv, const AutoFilterArgument* inputs, std::chrono::nanoseconds duration) {
  auto stub = std::make_shared<SignalStub>(*this, cv);

  // This ad-hoc filter detects when all the requested decorations have been added, and then
  // sets the necessary flags to inform the user
  AddRecipient(
    AutoFilterDescriptor(
      stub,
      AutoFilterDescriptorStub(
        auto_id_t<AutoPacketFactory>{},
        autowiring::altitude::Dispatch,
        inputs,
        false,
        [] (const void* pObj, AutoPacket&) {
          SignalStub* stub = (SignalStub*)pObj;

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
