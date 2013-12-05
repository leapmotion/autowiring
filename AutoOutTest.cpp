#include "stdafx.h"
#include "AutoOutTest.h"
#include "auto_out.h"
#include "TestFixtures/Decoration.h"

class SimpleFilter {
public:
  SimpleFilter(void) :
    hitCount(0)
  {}

  void AutoFilter(Decoration<0> in, auto_out<Decoration<1>> out) {
    hitCount++;
  }

  size_t hitCount;
};

TEST_F(AutoOutTest, VerifyNoMultiStrike) {
  AutoRequired<SimpleFilter> sf;
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
