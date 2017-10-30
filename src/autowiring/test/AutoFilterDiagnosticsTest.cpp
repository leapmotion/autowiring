// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/autowiring.h>
#include <autowiring/demangle.h>
#include <autowiring/SatCounter.h>

class AutoFilterDiagnosticsTest:
  public testing::Test
{
public:
  AutoFilterDiagnosticsTest(void) {
    AutoCurrentContext()->Initiate();
  }
};

class FilterMemberNotInheritingObject {
public:
  void AutoFilter(const int&) {}
};

TEST_F(AutoFilterDiagnosticsTest, CanGetExpectedTrueType) {
  AutoRequired<FilterMemberNotInheritingObject> mnio;
  AutoRequired<AutoPacketFactory> factory;

  auto packet = factory->NewPacket();
  auto decorations = packet->GetDecorations();
  ASSERT_EQ(1UL, decorations.size()) << "Dispositions collection contained an unexpected AutoFilter";

  auto& disposition = decorations.begin()->second;
  ASSERT_EQ(1UL, disposition.m_subscribers.size()) << "Expected exactly one subscriber for the sole present type";

  const SatCounter* descriptor = disposition.m_subscribers.begin()->satCounter;
  AnySharedPointer asp(descriptor->GetAutoFilter());

  // Get more information about this object from the enclosing context:
  AutoCurrentContext ctxt;
  auto_id id;
  ASSERT_NO_THROW(id = ctxt->GetAutoTypeId(asp)) << "Exception thrown while attempting to get true type information";
  ASSERT_EQ(auto_id_t<FilterMemberNotInheritingObject>{}, id) << "True type not correctly reported, got " << autowiring::demangle(id) << ", expected FilterMemberNotInheritingObject";
}
