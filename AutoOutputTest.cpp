#include "stdafx.h"
#include "AutoOutputTest.h"
#include "AutoPacketFactory.h"
#include "TestFixtures/Decoration.h"

class SimpleAutoOut {
public:
  SimpleAutoOut(void) :
    m_called(false),
    m_rcvDecAddr(nullptr)
  {}

  void AutoFilter(const Decoration<0>& decoration, auto_out<Decoration<1>> out1) {
    m_called = true;

    // Record the decoration address:
    m_rcvDecAddr = out1;

    // Satisfy our other decoration:
    if(out1)
      out1->i = -100;
  }

  bool m_called;
  Decoration<1>* m_rcvDecAddr;
};

class CompoundAutoOut {
public:
  CompoundAutoOut(void) :
    m_called(false),
    m_rcvDecAddr1(nullptr),
    m_rcvDecAddr2(nullptr)
  {}

  void AutoFilter(const Decoration<0>& decoration, auto_out<Decoration<1>> out1, auto_out<Decoration<2>> out2) {
    m_called = true;

    // Record the decoration address:
    m_rcvDecAddr1 = out1;
    m_rcvDecAddr2 = out2;

    // Satisfy our other decoration:
    if(out1)
      out1->i = -100;
    if(out2)
      out2->i = -101;
  }

  bool m_called;
  Decoration<1>* m_rcvDecAddr1;
  Decoration<2>* m_rcvDecAddr2;
};

template<int N>
class SimpleRequestor {
public:
  SimpleRequestor(void):
    m_called(false)
  {}

  void AutoFilter(const Decoration<N>& dec) {
    m_called = true;
  }

  bool m_called;
};

TEST_F(AutoOutputTest, VerifySimpleInput) {
  // Create our elements:
  Autowired<AutoPacketFactory> factory;
  AutoRequired<SimpleAutoOut> autoOut;
  AutoRequired<SimpleRequestor<1>> req;

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

TEST_F(AutoOutputTest, VerifyOptionalSatisfaction) {
  // Create our elements:
  Autowired<AutoPacketFactory> factory;
  AutoRequired<SimpleAutoOut> autoOut;

  // Trigger the SimpleAutoOut:
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>());

  // Verify that the SimpleAutoOut was correctly notified that its output was unneeded:
  EXPECT_EQ(nullptr, autoOut->m_rcvDecAddr) << "AutoOut incorrectly indicated that a decoration was needed, when it was not";
}

TEST_F(AutoOutputTest, VerifyMultiOutputSatisfaction) {
  // Create our elements:
  Autowired<AutoPacketFactory> factory;
  AutoRequired<CompoundAutoOut> autoOut;
  AutoRequired<SimpleRequestor<1>> req1;

  {
    // Trigger the SimpleAutoOut:
    auto packet = factory->NewPacket();
    packet->Decorate(Decoration<0>());

    // Verify that the SimpleRequestor got a packet as expected:
    EXPECT_TRUE(req1->m_called) << "Simple requestor did not receive a call as expected";

    // Verify that one of the two packets was supplied, and the other was not:
    EXPECT_NE(nullptr, autoOut->m_rcvDecAddr1) << "A decoration was not supplied when it had a subscription";
    EXPECT_EQ(nullptr, autoOut->m_rcvDecAddr2) << "A decoration was supplied when there were no subscribers";
  }

  // Introduce a requestor interested in the other output:
  AutoRequired<SimpleRequestor<2>> req2;

  {
    // Issue another packet:
    auto packet = factory->NewPacket();
    packet->Decorate(Decoration<0>());

    // Verify that BOTH requestors got called:
    EXPECT_TRUE(req1->m_called) << "First requestor was not invoked on an output type";
    EXPECT_TRUE(req2->m_called) << "First requestor was not invoked on an output type";

    // Verify that both outputs were satisfied:
    EXPECT_NE(nullptr, autoOut->m_rcvDecAddr1);
    EXPECT_NE(nullptr, autoOut->m_rcvDecAddr2);
  }
}