// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/autowiring.h>
#include "TestFixtures/Decoration.hpp"

class SatisfiabilityTest:
  public testing::Test
{
public:
  SatisfiabilityTest(void) {
    AutoCurrentContext()->Initiate();
  }

  AutoRequired<AutoPacketFactory> factory;
};

TEST_F(SatisfiabilityTest, MarkUnsatisfiableCalls) {
  auto packet = factory->NewPacket();

  // This filter shouldn't be called, because it expects a reference as an input
  bool bRefCalled = false;
  *packet += [&bRefCalled](const Decoration<0>& dec) { bRefCalled = true; };

  // This one should be called because the input is a shared pointer
  bool bSharedPtrCalled = false;
  *packet += [&bSharedPtrCalled](std::shared_ptr<const Decoration<0>> dec) { bSharedPtrCalled = true; };

  packet->MarkUnsatisfiable<Decoration<0>>();

  ASSERT_FALSE(bRefCalled) << "Reference version should not have been called";
  ASSERT_TRUE(bSharedPtrCalled) << "Shared pointer version should have been called as a result of Unsatisfiable";
}
