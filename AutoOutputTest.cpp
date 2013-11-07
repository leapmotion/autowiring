#include "stdafx.h"
#include "AutoOutputTest.h"
#include "AutoPacketFactory.h"
#include "TestFixtures/Decoration.h"

class SimpleAutoOut {
public:
  SimpleAutoOut(void):
    m_called(false),
    m_rcvDecAddr(nullptr)
  {}

  void AutoFilter(const Decoration<0>& decoration, auto_out<Decoration<1>> out1) {
    m_called = true;

    // Record the decoration address:
    m_rcvDecAddr = out1;

    // Satisfy our other decoration:
    out1->i = -100;
  }

  bool m_called;
  Decoration<1>* m_rcvDecAddr;
};

class SimpleRequestor {
public:
  SimpleRequestor(void):
    m_called(false)
  {}

  void AutoFilter(const Decoration<1>& dec) {
    m_called = true;
  }

  bool m_called;
};

TEST_F(AutoOutputTest, VerifySimpleInput) {
  // Create our elements:
  Autowired<AutoPacketFactory> factory;
  AutoRequired<SimpleAutoOut> autoOut;
  AutoRequired<SimpleRequestor> req;

  // Create the first decoration, this should trigger an autofilter invocation:
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>());

  EXPECT_TRUE(autoOut->m_called) << "AutoOut filter was not called as expected";

  const Decoration<1>* pDec;
  ASSERT_TRUE(packet->Get(pDec)) << "Packet did not receive a secondary decoration as expected";
  EXPECT_EQ(-100, pDec->i) << "Packet secondary decoration value was not obtained as expected";

  // Identity validation:
  EXPECT_EQ(pDec, autoOut->m_rcvDecAddr) << "Packet decoration must have been copied, address mismatch";

  // Verify secondary satisfaction:
  EXPECT_TRUE(req->m_called) << "Secondary satisfaction of an implicitly checked out decoration did not take place";
}