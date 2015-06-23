// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"

/*
 * We need to declare this before the test fixture, since the test fixture needs to
 * reset a static boolean flag when this test finishes. A static flag must be used
 * because were a testing a static 'new' function.
*/
class HasCustomNewFunction {
public:
  static bool s_invoked;

  static void* operator new(size_t ncb){
    s_invoked = true;

    uint8_t* pRetVal = ::new uint8_t[ncb];
    for (size_t i = 0; i < ncb; i++)
      pRetVal[i] = (i + 1) * 101;
    return pRetVal;
  }

  static void operator delete(void* ptr) {
    ::delete[] static_cast<uint8_t*>(ptr);
  }

  uint8_t data[128];
};
bool HasCustomNewFunction::s_invoked = false;

class AutoFilterConstructRulesTest:
  public testing::Test
{
public:
  AutoFilterConstructRulesTest(void) {
    // All decorator tests must run from an initiated context
    AutoCurrentContext()->Initiate();
  }

protected:
  virtual void SetUp() {
    // Make sure static flags are in a good state before the test starts
    HasCustomNewFunction::s_invoked = false;
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

class GeneratesCustomAllocatedType {
public:
  void AutoFilter(HasCustomNewFunction&) {}
};

TEST_F(AutoFilterConstructRulesTest, CorrectlyCallsCustomAllocator) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<GeneratesCustomAllocatedType>();

  ASSERT_FALSE(HasCustomNewFunction::s_invoked) << "Custom allocator was invoked prematurely";
  auto packet = factory->NewPacket();
  auto* phcnf = packet->GetShared<HasCustomNewFunction>();
  auto& hcnf = *phcnf;
  ASSERT_TRUE(phcnf && hcnf) << "Decoration with custom allocator not present on a packet as expected";
  ASSERT_TRUE(HasCustomNewFunction::s_invoked) << "Custom new allocator was not invoked as expected";

  for (size_t i = 0; i < 128; i++)
    ASSERT_EQ(
      static_cast<uint8_t>((i + 1) * 101),
      hcnf->data[i]
    ) << "Custom new allocator did not fill space, mismatch at offset " << i;
}
