// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once

#include <unordered_map>
#include <vector>
#include <mutex>
#include <auto_id.h>

class AutoPacket;

class AutoPacketProfiler {
public:
  AutoPacketProfiler();

  // This is an ID for the class on which AutoFilter was called.
  typedef auto_id EventType;

  enum EventFlag {
    FlagError = 0x00,
    FlagBegin = 0x01, // Begin < End in sort for matching time stamps
    FlagEnd = 0x02,
  };

  struct Event {
    Event() : m_type(nullptr), m_flag(EventFlag::FlagError), m_time(0) { }
    Event(EventType type, EventFlag flags, int64_t time) : m_type(type), m_flag(flags), m_time(time) { }
    EventType m_type;
    EventFlag m_flag;
    int64_t   m_time;

    // sort key.
    bool operator<(const Event& rhs) const {
      return m_time != rhs.m_time ? (m_time < rhs.m_time) : (m_flag < rhs.m_flag);
    }
  };

  struct FrameEvent : public Event {
    FrameEvent() = delete;

    FrameEvent(int64_t packet_id, EventType type, EventFlag flag, int64_t time) :
      Event(type, flag, time),
      m_packet_id(packet_id) {}

    int64_t m_packet_id;
  };

  class Block {
  public:
    Block(AutoPacket* packet, EventType type);
    ~Block();

  private:
    AutoPacketProfiler* m_profiler;
    AutoPacket*  m_packet;
    EventType    m_type;
  };

  static int64_t GetNow();

  void Begin(AutoPacket* packet, EventType type, int64_t time = GetNow());
  void End(AutoPacket* packet, EventType type, int64_t time = GetNow());

  // Result may be modified or even deleted if the profiler is accessed by other threads.
  const std::vector<Event>* UnsafeCheckForEvents(int64_t packet_id);

  //  void SendAutoNetServerEvent();

private:


  // Access must be wrapped in m_Mutex to avoid a race condition on the first call.
  std::vector<Event>& GetEventList(int64_t packet_id);
  void AgeTable(int64_t current_packet_id);

  // Will not hold packet references
  inline void Submit(AutoPacket* packet, EventType type, EventFlag flag, int64_t time);

  std::mutex m_mutex;

  // m_packet_id -> Event
  std::vector<Event>* m_LastEventList;
  int64_t m_LastFrameId;

  int64_t m_AgeCounterStartFrameId;
  int m_AgeCounter;

  std::unordered_map<int64_t, std::vector<Event>> m_EventListTable;

  static auto_id_t<AutoPacketProfiler> m_overheadId;

  // output
  //  Value m_results;
};

