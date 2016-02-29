// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/CoreThread.h>
#include <autowiring/AutoPacketProfiler.h>

#include "TestFixtures/Decoration.hpp"

class AutoPacketProfilerTest :
  public testing::Test
{
public:
  AutoPacketProfilerTest(void) {
    AutoCurrentContext()->Initiate();

    factory->EnableAutoPacketProfiler(true);
  }

  ~AutoPacketProfilerTest(void) {
    factory->EnableAutoPacketProfiler(false);
  }

  AutoRequired<AutoPacketFactory> factory;
};

TEST_F(AutoPacketProfilerTest, EnableControls) {
  ASSERT_TRUE(factory->GetAutoPacketProfiler() != nullptr) << "Expected AutoPacketProfiler created by EnableAutoPacketProfiler";

  factory->EnableAutoPacketProfiler(false);
  ASSERT_TRUE(factory->GetAutoPacketProfiler() == nullptr) << "Expected AutoPacketProfiler destroyed by EnableAutoPacketProfiler";

  factory->EnableAutoPacketProfiler(true);
  ASSERT_TRUE(factory->GetAutoPacketProfiler() != nullptr) << "Expected AutoPacketProfiler created again by EnableAutoPacketProfiler";
}

template<int N>
class SimpleAutoFilter {
public:
  int m_value = 0;
  void AutoFilter(int value) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    m_value = value;
  }
};

TEST_F(AutoPacketProfilerTest, CheckForEventRecordings) {
  AutoCurrentContext ctxt;

  AutoRequired<SimpleAutoFilter<0>> simpleFilter0(ctxt);
  AutoRequired<SimpleAutoFilter<1>> simpleFilter1(ctxt);
  AutoRequired<SimpleAutoFilter<2>> simpleFilter2(ctxt);

  for (int count = 0; count < 7; ++count) {
    auto trivial = factory->NewPacket();
    trivial->SetUserId(345 + count);
    trivial->Decorate((int)(100 + count));
    ASSERT_EQ(100 + count, simpleFilter0->m_value) << "A simple packet was not received as expected by an AutoFilter";
    ASSERT_EQ(100 + count, simpleFilter1->m_value) << "A simple packet was not received as expected by an AutoFilter";
    ASSERT_EQ(100 + count, simpleFilter2->m_value) << "A simple packet was not received as expected by an AutoFilter";
  }

  AutoPacketProfiler& profiler = *factory->GetAutoPacketProfiler();

  for (int count = 0; count < 7; ++count) {
    AutoPacketProfiler::Access profilerAccess(profiler);

    // These will throw if missing
    int64_t uniqueId = profilerAccess.userIdToUniqueId.at(345 + count);
    const AutoPacketProfiler::Record& record = profilerAccess.eventListTable.at(uniqueId);
    ASSERT_TRUE(record.userId == 345 + count);

    // There will be begin and end events for the 3 above and the overhead sample for the profiler itself.
    ASSERT_TRUE(record.events.size() == 8) << "8 Events were not recorded";

    int countBegin = 0;
    int countEnd = 0;
    for (const auto& evt : record.events) {
      if (evt.m_flag == AutoPacketProfiler::FlagBegin) ++countBegin;
      if (evt.m_flag == AutoPacketProfiler::FlagEnd) ++countEnd;
    }

    ASSERT_TRUE(countBegin == 4);
    ASSERT_TRUE(countEnd == 4);
  }
}

