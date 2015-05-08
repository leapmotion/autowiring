// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"

class AutoFilterConstructRulesTest:
  public testing::Test
{
public:
  AutoFilterConstructRulesTest(void) {
    // All decorator tests must run from an initiated context
    AutoCurrentContext()->Initiate();
  }
};

class CannotBeDefaultConstructed {
  CannotBeDefaultConstructed(int) {}
};

class SharedPtrNoDefault
{
public:
  void AutoFilter(std::shared_ptr<CannotBeDefaultConstructed>& out) {
    ASSERT_EQ(nullptr, out) << "An argument that should have been provided null was incorrectly default constructed";
  }
};

TEST_F(AutoFilterConstructRulesTest, SharedPtrNoDefaultTest) {
  AutoRequired<SharedPtrNoDefault> spnd;
  AutoRequired<AutoPacketFactory> factory;
  auto packet = factory->NewPacket();
}

class WantsAutoPacketInput {
public:
  WantsAutoPacketInput(void):
    pPacket(nullptr)
  {}

  WantsAutoPacketInput(AutoPacket& packet):
    pPacket(&packet)
  {}

  AutoPacket* pPacket;
};

class ConstructsWantsAutoPacketInput {
public:
  void AutoFilter(AutoPacket& packet, WantsAutoPacketInput& wapi) {
    ASSERT_EQ(&packet, wapi.pPacket) << "Speculatively constructed output type did not have the correct constructor overload invoked";
  }
};

TEST_F(AutoFilterConstructRulesTest, CtorRequiredWPI) {
  // This is enough to kick off the AutoFilter above and cause an exception, if one is going to occur
  AutoRequired<ConstructsWantsAutoPacketInput>();
  AutoRequired<AutoPacketFactory>()->NewPacket();
}

class UnnamedExternalClass;

class AcceptsUnnamedExternalClass {
public:
  void AutoFilter(const UnnamedExternalClass&) {}
};

class AcceptsUnnamedExternalClassSharedPtr {
public:
  void AutoFilter(std::shared_ptr<const UnnamedExternalClass>) {}
};

TEST_F(AutoFilterConstructRulesTest, CanAcceptUndefinedSharedPointerInput) {
  AutoRequired<AcceptsUnnamedExternalClass> auec;
  AutoRequired<AcceptsUnnamedExternalClassSharedPtr> auecsp;
}
