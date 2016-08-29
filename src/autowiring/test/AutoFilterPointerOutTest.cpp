// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"

class AutoFilterPointerOut:
  public testing::Test
{
public:
  AutoFilterPointerOut(void) {
    AutoCurrentContext()->Initiate();
  }
};

TEST_F(AutoFilterPointerOut, SingleDecorationNotConsumed) {
  AutoRequired<AutoPacketFactory> factory;

  bool called = false;
  bool hasValue = false;
  *factory += [&] (Decoration<0>* pOut) {
    called = true;
    hasValue = !!pOut;
  };

  auto packet = factory->NewPacket();
  ASSERT_TRUE(called);
  ASSERT_FALSE(hasValue) << "Decoration<0> was not correctly identified as having no subscribers";
}

TEST_F(AutoFilterPointerOut, SingleSubscriber) {
  AutoRequired<AutoPacketFactory> factory;

  bool called = false;
  bool hasValue = false;
  *factory += [&](Decoration<0>* pOut) {
    called = true;
    hasValue = !!pOut;
    if(pOut)
      *pOut = { 1766 };
  };

  Decoration<0> value;
  bool downstreamCalled = false;
  *factory += [&](Decoration<0> downstream) {
    downstreamCalled = true;
    value = downstream;
  };

  auto packet = factory->NewPacket();
  ASSERT_TRUE(called);
  ASSERT_TRUE(hasValue) << "Decoration<0> was not correctly identified as having one subscribers";
  ASSERT_TRUE(downstreamCalled) << "Downstream filter not called as expected";
  ASSERT_EQ(1766, value.i);
}

TEST_F(AutoFilterPointerOut, ConstPointerInput) {
  AutoRequired<AutoPacketFactory> factory;

  bool called = false;
  Decoration<0> value{ -1 };
  *factory += [&](const Decoration<0>* pIn) {
    called = true;
    value = *pIn;
  };

  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>{4124});
  ASSERT_TRUE(called);
  ASSERT_EQ(4124, value.i) << "Const input pointer decoration was not called as expected";
}

TEST_F(AutoFilterPointerOut, PointerToSharedPointerNoSubcriber) {
  AutoRequired<AutoPacketFactory> factory;

  bool called = false;
  bool hasPtrValue = false;
  *factory += [&](std::shared_ptr<Decoration<0>>* ppOut) {
    hasPtrValue = !!ppOut;
    called = true;
  };

  auto packet = factory->NewPacket();
  ASSERT_TRUE(called) << "Shared pointer decoration filter not called as expected";
  ASSERT_FALSE(hasPtrValue) << "Pointer to shared pointer should have been null";
}

TEST_F(AutoFilterPointerOut, PointerToSharedPointer) {
  AutoRequired<AutoPacketFactory> factory;

  bool called = false;
  *factory += [&](std::shared_ptr<Decoration<0>>* ppOut) {
    *ppOut = std::make_shared<Decoration<0>>(92941);
    called = true;
  };
  *factory += [&](Decoration<0>) {};

  auto packet = factory->NewPacket();
  ASSERT_TRUE(called) << "Shared pointer decoration filter not called as expected";
  ASSERT_TRUE(packet->Has<Decoration<0>>()) << "Decoration not present on the packet as expected";

  std::shared_ptr<const Decoration<0>> dec;
  packet->Get(dec);
  ASSERT_EQ(92941, dec->i);
}
