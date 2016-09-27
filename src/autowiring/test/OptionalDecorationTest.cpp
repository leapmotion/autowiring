// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "OptionalDecorationTest.h"
#include "AutoPacketFactory.h"
#include "TestFixtures/Decoration.h"

class OptionalFilter {
public:
  OptionalFilter(void) :
    m_hadOptional(false),
    m_called(false)
  {}

  void AutoFilter(optional_ptr<Decoration<0>> optional, Decoration<1> required) {
    m_called = true;
    m_hadOptional = optional;
  }

  bool m_hadOptional;
  bool m_called;
};

TEST_F(OptionalDecorationTest, EnsureOrdinarySatisfaction) {
  AutoRequired<OptionalFilter> filter;
  AutoRequired<AutoPacketFactory> factory;

  // Obtain a new packet and verify that ordinary satisfaction works
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>());
  packet->Decorate(Decoration<1>());

  EXPECT_TRUE(filter->m_hadOptional) << "Expected, but did not receive, an optional pointer";
  EXPECT_TRUE(filter->m_called) << "Optional decoration was not correctly parsed";
}

TEST_F(OptionalDecorationTest, EnsureOmittedSatisfaction) {
  AutoRequired<OptionalFilter> filter;
  AutoRequired<AutoPacketFactory> factory;

  {
    // Obtain a new packet and only add the mandatory decoration:
    auto packet = factory->NewPacket();
    packet->Decorate(Decoration<1>());

    // Adding the mandatory decoration should not have been adequate to satisfy the input:
    EXPECT_FALSE(filter->m_called) << "Filter called before the packet was released";
  }

  EXPECT_FALSE(filter->m_hadOptional) << "Optional pointer was passed even though no optional decoration was set";
  EXPECT_TRUE(filter->m_called) << "Optional call was not correctly made";
}

TEST_F(OptionalDecorationTest, VerifyEarlyOptOutSatisfaction) {
  AutoRequired<OptionalFilter> filter;
  AutoRequired<AutoPacketFactory> factory;

  // Obtain a new packet and add the mandatory decoration:
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<1>());

  // Verify that we cannot unsatisfy something that was already decorated:
  EXPECT_THROW(packet->Unsatisfiable<Decoration<1>>(), std::runtime_error) << "Antidecoration did not fail for an already-existing decoration";

  // Now manually unsatisfy with the optional decoration:
  packet->Unsatisfiable<Decoration<0>>();

  // This should have satisfied this requirement and should not have passed the optional field:
  EXPECT_TRUE(filter->m_called) << "Filter was not invoked as expected when a decoration was contraindicated";
  EXPECT_FALSE(filter->m_hadOptional) << "Filter received the optional field, but this field was incorrectly assigned";
}

TEST_F(OptionalDecorationTest, VerifyCancelledCheckoutSatisfaction) {
  AutoRequired<OptionalFilter> filter;
  AutoRequired<AutoPacketFactory> factory;

  // Obtain a new packet and add the mandatory decoration:
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<1>());

  // Allow a checkout to expire:
  packet->Checkout<Decoration<0>>();

  // This should have satisfied this requirement and should not have passed the optional field:
  EXPECT_TRUE(filter->m_called) << "Filter was not invoked as expected when a checkout expired";
  EXPECT_FALSE(filter->m_hadOptional) << "Filter received the optional field, but this field was incorrectly assigned";
}
