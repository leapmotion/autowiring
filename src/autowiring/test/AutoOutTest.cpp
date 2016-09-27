// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoOutTest.h"
#include "auto_out.h"
#include "TestFixtures/Decoration.h"

template<bool auto_ready>
class SimpleFilter {
public:
  SimpleFilter(void) :
    hitCount(0)
  {}

  void AutoFilter(Decoration<0> in, auto_out<Decoration<1>, auto_ready> out) {
    hitCount++;
  }

  size_t hitCount;
};

template<bool auto_ready>
static void Validate(void) {
}

TEST_F(AutoOutTest, VerifyNoMultiStrikeNoAutoReady) {
  AutoRequired<SimpleFilter<false>> sf;
  AutoRequired<FilterG> recipient;

  // Simple decoration case, SimpleFilter should supply the other decoration to eventuate the recipient:
  Autowired<AutoPacketFactory> factory;
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>());

  // Verify our simple filter got hit:
  ASSERT_EQ(1UL, sf->hitCount) << "Filter was hit too many times when it only generated a single decoration";

  // Verify the recipient hasn't been hit yet:
  ASSERT_FALSE(recipient->m_called) << "Recipient was invoked prematurely";

  // Now add the final decoration and verify the recipient STILL was not hit:
  packet->Decorate(Decoration<2>());
  ASSERT_FALSE(recipient->m_called) << "Recipient was invoked even though one of its inputs should have been unsatisfiable";
}

TEST_F(AutoOutTest, VerifyNoMultiStrikeAutoReady) {
  AutoRequired<SimpleFilter<true>> sf;
  AutoRequired<FilterG> recipient;

  // Simple decoration case, SimpleFilter should supply the other decoration to eventuate the recipient:
  Autowired<AutoPacketFactory> factory;
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>());

  // Verify our simple filter got hit:
  ASSERT_EQ(1UL, sf->hitCount) << "Filter was hit too many times when it only generated a single decoration";

  // Verify the recipient hasn't been hit yet:
  ASSERT_FALSE(recipient->m_called) << "Recipient was invoked prematurely";

  // Now add the final decoration and perform another check:
  packet->Decorate(Decoration<2>());
  ASSERT_TRUE(recipient->m_called) << "Recipient was not invoked even though all of its inputs were satisfied";
  ASSERT_EQ(1UL, recipient->m_hitCount) << "Recipient received a single event more than once";
}
