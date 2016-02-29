// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "chrono_types.h"
#include "AutoPacketProfiler.h"
#include "AutoPacket.h"
#include "autowiring_error.h"

using namespace std::chrono;
using namespace autowiring;

namespace {
  static const int sc_FrameTimelineEventListCapacity = 1024; // Number of events for a frame before reallocation
  static const int sc_FrameTimelineMinTableSize = 100;       // Number of frames before old frames are released.
}

// ----------------------------------------------------------------------------
// inline

inline void AutoPacketProfiler::Submit(AutoPacket* packet, EventType type, EventFlag flag, int64_t time) {
  std::lock_guard<std::mutex> lock(m_mutex);

  int64_t id = packet->GetUniqueId();

  // GetEventList can trigger a lot of memory allocation traffic.
  Record& record = (m_LastFrameId == id) ? *m_LastRecord : GetRecord(id);
  record.events.emplace_back(type, flag, time);

  // Allow user IDs to be associated with a packet at any time.
  if(record.userId != packet->GetUniqueId()) {
    record.userId = packet->GetUniqueId();
    m_UserIdToUniqueId[id] = record.userId;
  }
}

// ----------------------------------------------------------------------------
// AutoPacketProfiler::Block

AutoPacketProfiler::Block::Block(AutoPacket* packet, EventType type) :
  m_profiler(packet->GetProfiler()),
  m_packet(packet),
  m_type(type)
{
  m_profiler->Submit(packet, type, AutoPacketProfiler::FlagBegin, GetNow());
}

AutoPacketProfiler::Block::~Block()
{
  m_profiler->Submit(m_packet, m_type, AutoPacketProfiler::FlagEnd, GetNow());
}

// ----------------------------------------------------------------------------
// AutoPacketProfiler

auto_id_t<AutoPacketProfiler> AutoPacketProfiler::s_overheadId;

int64_t AutoPacketProfiler::GetNow() {
  return duration_cast<microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

void AutoPacketProfiler::Begin(AutoPacket* packet, EventType type, int64_t time) {
  Submit(packet, type, AutoPacketProfiler::FlagBegin, time);
}

void AutoPacketProfiler::End(AutoPacket* packet, EventType type, int64_t time) {
  Submit(packet, type, AutoPacketProfiler::FlagEnd, time);
}

AutoPacketProfiler::AutoPacketProfiler() :
  m_LastRecord(nullptr),
  m_LastFrameId(-1),
  m_AgeCounterStartFrameId(-1),
  m_AgeCounter(0)
{
}

AutoPacketProfiler::Record& AutoPacketProfiler::GetRecord(int64_t packet_id) {

  m_LastRecord = &m_EventListTable[packet_id];
  m_LastFrameId = packet_id;

  // This can get really slow due to deallocations.  The idea is to only do this very rarely.
  if (m_LastRecord->events.empty()) {
    int64_t time = GetNow();

    m_LastRecord->events.reserve(sc_FrameTimelineEventListCapacity);
    AgeTable(packet_id);

    // AgeTable could be bad enough that this will avoid confusion.  Might as well declare the array allocation
    // to provide a baseline measurement as well.  

    m_LastRecord->events.emplace_back(s_overheadId, AutoPacketProfiler::FlagBegin, time);
    m_LastRecord->events.emplace_back(s_overheadId, AutoPacketProfiler::FlagEnd, GetNow());
  }

  return *m_LastRecord;
}

void AutoPacketProfiler::AgeTable(int64_t current_packet_id) {
  ++m_AgeCounter;
  if (m_AgeCounter > sc_FrameTimelineMinTableSize) {
    for (auto it = m_EventListTable.begin(); it != m_EventListTable.end();) {
      if (it->first <= m_AgeCounterStartFrameId) {
        it = m_EventListTable.erase(it);
      }
      else
        ++it;
    }

    m_AgeCounter = 0;
    m_AgeCounterStartFrameId = current_packet_id;
  }

  // Just rebuild m_UserIdToUniqueId.
  m_UserIdToUniqueId.clear();

  for (auto& i : m_EventListTable)
    if (i.second.userId != -1)
      m_UserIdToUniqueId[i.second.userId] = i.first;
}

void AutoPacketProfiler::SortUnsafe() {
  for (auto& it : m_EventListTable) {
    std::vector<Event>& eventList = it.second.events;
    std::sort(eventList.begin(), eventList.end());
  }
}
