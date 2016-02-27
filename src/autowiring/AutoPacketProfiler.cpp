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

  int64_t id = packet->GetUserId();

  // TODO: The packet's unique ID would be usable here, however a mapping would need to be kept to the User ID.
//  if(id == -1)
//    throw autowiring_error("The AutoPacketProfiler requires a User ID to be set for the packet.");

  if (id == -1)
    return; // TODO: AutoPacketInternal::Initialize might call filters before client code can set a user ID.

  // GetEventList can trigger a lot of memory allocation traffic.
  std::vector<Event>& events = (m_LastFrameId == id) ? *m_LastEventList : GetEventList(id);
  events.emplace_back(type, flag, time);
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

auto_id_t<AutoPacketProfiler> AutoPacketProfiler::m_overheadId;

int64_t AutoPacketProfiler::GetNow() {
  return duration_cast<microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

void AutoPacketProfiler::Begin(AutoPacket* packet, EventType type, int64_t time) {
  Submit(packet, type, AutoPacketProfiler::FlagBegin, time);
}

void AutoPacketProfiler::End(AutoPacket* packet, EventType type, int64_t time) {
  Submit(packet, type, AutoPacketProfiler::FlagEnd, time);
}

const std::vector<AutoPacketProfiler::Event>* AutoPacketProfiler::UnsafeCheckForEvents(int64_t packet_id) {
  std::lock_guard<std::mutex> lock(m_mutex);
  auto it = m_EventListTable.find(packet_id);
  if (it != m_EventListTable.end()) {
    std::sort(it->second.begin(), it->second.end());
    return &it->second;
  }
  return nullptr;
}

AutoPacketProfiler::AutoPacketProfiler() :
  m_LastEventList(nullptr),
  m_LastFrameId(-1),
  m_AgeCounterStartFrameId(-1),
  m_AgeCounter(0)
{
}

std::vector<AutoPacketProfiler::Event>& AutoPacketProfiler::GetEventList(int64_t packet_id) {

  m_LastEventList = &m_EventListTable[packet_id];
  m_LastFrameId = packet_id;

  // This can get really slow due to deallocations.  The idea is to only do this very rarely.
  if (m_LastEventList->empty()) {
    int64_t time = GetNow();

    m_LastEventList->reserve(sc_FrameTimelineEventListCapacity);
    AgeTable(packet_id);

    // AgeTable could be bad enough that this will avoid confusion.  Might as well declare the array allocation
    // to provide a baseline measurement as well.  

    m_LastEventList->emplace_back(m_overheadId, AutoPacketProfiler::FlagBegin, time);
    m_LastEventList->emplace_back(m_overheadId, AutoPacketProfiler::FlagEnd, GetNow());
  }

  return *m_LastEventList;
}

void AutoPacketProfiler::AgeTable(int64_t current_packet_id) {
  ++m_AgeCounter;
  if (m_AgeCounter > sc_FrameTimelineMinTableSize) {

    for (auto it = m_EventListTable.begin(); it != m_EventListTable.end();) {
      if (it->first <= m_AgeCounterStartFrameId) {
        it = m_EventListTable.erase(it);
      }
      else {
        ++it;
      }
    }

    m_AgeCounter = 0;
    m_AgeCounterStartFrameId = current_packet_id;
  }
}

/*
void AutoPacketProfiler::SendAutoNetServerEvent() {
  if (m_autoNetServer) {

    // Dump previous sc_FrameTimelineMinTableSize frames.
    m_results = Value::Hash();

    Value::Array frameTable;
    m_results.HashSet("frameTable", frameTable);

    std::vector<std::vector<Event*>> perFrameRows;

    for (auto& it : m_EventListTable) {
      std::vector<Event>& eventList = it.second;
      std::sort(eventList.begin(), eventList.end());

      // Be reasonable wrt. to reallocation
      for (auto& row : perFrameRows) {
        row.clear();
      }

      for (auto& event : eventList) {
        if (event.m_flag == FlagBegin) {
          // Find first row that does not have an open range.
          bool found = false;
          for (auto& row : perFrameRows) {
            if (row.empty() || row.back()->m_flag == FlagEnd) {
              row.push_back(&event);
              found = true;
            }
          }
          if (!found) {
            perFrameRows.emplace_back();
            perFrameRows.back().push_back(&event);
          }
        }
        else if (event.m_flag == FlagEnd) {
          // Find row that has this range.
          bool found = false;
          for (auto& row : perFrameRows) {

            // Assuming that there is only one event of the same type executing per-frame id.

            if (!row.empty() && (strcmp(row.back()->m_type, event.m_type) == 0) && ) {
              row.push_back(&event);
              found = true;
            }
          }
          if (!found) {
            perFrameRows.emplace_back();
            perFrameRows.back().push_back(&event);
          }
        }
      }


      /*

      Value::Array frameEventRows;

      free_rows.clear();

      for (auto& event : li.second) {

      Value v;
      v.HashSet("flag", event.m_flag);
      v.HashSet("time", event.m_time);
      v.HashSet("type", event.m_type);

      if (event.m_flag == FlagBegin) {
      if (free_rows.empty()) {

      frameEventRows.emplace_back();
      frameEventRows.back().emplace_back()

      }


      }

      // Push "begin" events on a stack.  Each element of the stack is a row that
      // is being colored in horizontally.  

    }

    m_autoNetServer->SendEvent("sequence", m_results.ToJSON());
  }
}
*/
