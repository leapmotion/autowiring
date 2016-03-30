// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/autowiring.h>
#include "TestFixtures/Decoration.hpp"

class AutoFilterSatisfiabilityTest :
  public testing::Test
{
public:
  AutoFilterSatisfiabilityTest(void) {
    AutoCurrentContext()->Initiate();
  }

  AutoRequired<AutoPacketFactory> factory;
};

TEST_F(AutoFilterSatisfiabilityTest, MarkUnsatisfiableCalls) {
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

TEST_F(AutoFilterSatisfiabilityTest, TransitiveUnsatisfiability) {
  // Set up the filter configuration that will create the transitive condition
  auto called0 = std::make_shared<bool>(false);
  *factory += [called0](Decoration<0> in, std::shared_ptr<Decoration<1>>& out) {
    *called0 = true;
  };

  // This filter accepts Decoration<1> as an optional input argument.  It should be called
  // with this value set to nullptr.
  auto called1 = std::make_shared<bool>(false);
  *factory += [called1](std::shared_ptr<const Decoration<1>> in) {
    *called1 = true;
  };

  // This filter won't be called at all
  *factory += [] (Decoration<1>, Decoration<2>&) {};

  // This verifies that we do have correct transitive unsatisfiability behavior
  auto called2 = std::make_shared<bool>(false);
  *factory += [called2] (std::shared_ptr<const Decoration<2>>) {
    *called2 = true;
  };

  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>{});
  ASSERT_TRUE(packet->IsUnsatisfiable<Decoration<1>>());
  ASSERT_TRUE(*called1);
  ASSERT_TRUE(packet->IsUnsatisfiable<Decoration<2>>());
  ASSERT_TRUE(*called2);
}

TEST_F(AutoFilterSatisfiabilityTest, RvalueMarkUnsatisfiableTest) {
  auto refCalled = std::make_shared<bool>(false);
  *factory += [refCalled] (Decoration<1>&& ref) {
    *refCalled = true;
  };

  auto sharedCalled = std::make_shared<bool>(false);
  *factory += [sharedCalled] (std::shared_ptr<Decoration<1>>&& sp) {
    sp.reset(new Decoration<1>);
    *sharedCalled = true;
  };

  auto packet = factory->NewPacket();
  packet->MarkUnsatisfiable<Decoration<1>>();
  ASSERT_FALSE(*refCalled);
  ASSERT_TRUE(*sharedCalled);
}

TEST_F(AutoFilterSatisfiabilityTest, RvalueTransitiveUnsatisfiability) {
  auto called0 = std::make_shared<bool>(false);
  *factory += [called0] (Decoration<0> in, std::shared_ptr<Decoration<1>>& out) {
    out.reset(new Decoration<1>);
    out->i = in.i;
    *called0 = true;
  };

  auto called1 = std::make_shared<bool>(false);
  *factory += [called1] (std::shared_ptr<Decoration<1>>&& sp) {
    sp.reset();
    *called1 = true;
  };

  // This filter won't be called at all
  auto called2 = std::make_shared<bool>(false);
  *factory += [called2] (Decoration<1>, Decoration<2>&) {
    *called2 = true;
  };

  // This verifies that we do have correct transitive unsatisfiability behavior
  auto called3 = std::make_shared<bool>(false);
  *factory += [called3] (std::shared_ptr<Decoration<2>>&&) {
    *called3 = true;
  };

  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>{});
  ASSERT_TRUE(*called0);
  ASSERT_TRUE(*called1);
  ASSERT_FALSE(*called2) << "Reference version should not be called after Decoration<1> becomes unsatisfiable";
  ASSERT_TRUE(*called3);
  ASSERT_TRUE(packet->IsUnsatisfiable<Decoration<1>>()) << "Decoration<1> is not successfully marked as unsatisfiable";
  ASSERT_TRUE(packet->IsUnsatisfiable<Decoration<2>>()) << "Decoration<2> is not successfully marked as unsatisfiable";
}
