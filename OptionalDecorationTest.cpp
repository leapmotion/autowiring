#include "stdafx.h"
#include "OptionalDecorationTest.h"
#include "AutoPacketFactory.h"
#include "TestFixtures/Decoration.h"

class OptionalFilter {
public:
  OptionalFilter(void) :
    m_called(false)
  {}

  void AutoFilter(optional_ptr<Decoration<0>> optional, Decoration<1> required) {
  }

  bool m_called;
};

TEST_F(OptionalDecorationTest, EnsureOrdinarySatisfaction) {
  AutoRequired<OptionalFilter> filter;
  AutoRequired<AutoPacketFactory> factory;

  // Obtain a new packet and verify that ordinary satisfaction works
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>());
  packet->Decorate(Decoration<1>());

  EXPECT_TRUE(filter->m_called) << "Optional decoration was not correctly parsed";
}