// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ImmediateDecoratorTest.h"
#include "AutoPacketFactory.h"
#include "TestFixtures/Decoration.h"

TEST_F(ImmediateDecoratorTest, VerifyImmediate) {
  // Create one filter which cannot be immediately satisfied, and one which can be:
  AutoRequired<FilterA> filterA;
  AutoRequired<FilterF> filterF;

  AutoRequired<AutoPacketFactory> factory;
  auto packet = factory->NewPacket();

  // Create an immediate style decoration:
  Decoration<0> imm;
  packet->DecorateImmediate(&imm);
  EXPECT_TRUE(filterF->m_called) << "FilterF should have been satisfied immediately, but it was not";

  // Now add the other decoration:
  packet->Decorate(Decoration<1>());

  // Verify that the first filter was not satisfied:
  EXPECT_FALSE(filterA->m_called) << "FilterA was called, even though it had no opportunity to be called";
}

TEST_F(ImmediateDecoratorTest, VerifyNoImmediateCrossthread) {
  // Create a concurrent filter which should not get called:
  AutoRequired<FilterB> filterB;
  m_create->InitiateCoreThreads();

  // Decorate a packet with half of the filter's requirement:
  AutoRequired<AutoPacketFactory> factory;
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>());

  // Verify that the factory correctly recognizes a deferred tag:
  auto desc = factory->FindSubscriber<FilterB>();
  ASSERT_TRUE(desc != nullptr) << "Failed to find a known subscriber";
  ASSERT_TRUE(desc->IsDeferred()) << "Deferred filter incorrectly identified as being non-deferred";

  // Now, use an immediate decoration on the other half, and wait for the filter to quit:
  Decoration<1> dec;
  packet->DecorateImmediate(&dec);

  // We should not hit the second filter's satisfaction:
  filterB->m_barr.wait();
  *filterB += [&filterB] { filterB->Stop(); };
  filterB->Wait();
  EXPECT_FALSE(filterB->m_excepted) << "Parallel filter threw an exception while processing a message";
  EXPECT_FALSE(filterB->m_called) << "Parallel filter incorrectly received an immediate-mode decoration";
}
