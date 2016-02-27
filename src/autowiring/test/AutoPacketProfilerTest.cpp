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
  void AutoFilter(int value) { m_value = value; }
};

TEST_F(AutoPacketProfilerTest, AutoPacketFactoryCycle) {
  AutoCurrentContext ctxt;

  AutoRequired<SimpleAutoFilter<0>> simpleFilter0(ctxt);
  AutoRequired<SimpleAutoFilter<1>> simpleFilter1(ctxt);
  AutoRequired<SimpleAutoFilter<2>> simpleFilter2(ctxt);

  for (int count = 0; count < 7; ++count) {
    auto trivial = factory->NewPacket();
    trivial->SetUserId(345 + count);
    trivial->Decorate((int)100);
    ASSERT_EQ(100 + count, simpleFilter0->m_value) << "A simple packet was not received as expected by an AutoFilter";
    ASSERT_EQ(100 + count, simpleFilter1->m_value) << "A simple packet was not received as expected by an AutoFilter";
    ASSERT_EQ(100 + count, simpleFilter2->m_value) << "A simple packet was not received as expected by an AutoFilter";
  }

  AutoPacketProfiler* profiler = factory->GetAutoPacketProfiler();

  for (int count = 0; count < 7; ++count) {
    const std::vector<AutoPacketProfiler::Event>* events = profiler->UnsafeCheckForEvents(345 + count);
    ASSERT_TRUE(events != nullptr) << "Events were not recorded";
    ASSERT_TRUE(events->size() == 6) << "6 Events were not recorded";

    bool expectBegin = true;
    for (const auto& evt : *events) {
      ASSERT_TRUE((evt.m_flag == AutoPacketProfiler::FlagBegin) == expectBegin);
      expectBegin = !expectBegin;
    }
  }

  ASSERT_TRUE(true) << "X";
}
