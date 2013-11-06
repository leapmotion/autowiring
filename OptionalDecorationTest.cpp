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