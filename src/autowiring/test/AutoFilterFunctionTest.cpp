// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"
#include <autowiring/autowiring.h>

class AutoFilterFunctionalTest:
  public testing::Test
{
public:
  AutoFilterFunctionalTest(void) {
    AutoCurrentContext()->Initiate();
  }
};

void FilterFunction(const Decoration<0>& typeIn, auto_out<Decoration<1>> typeOut) {
  typeOut->i += 1 + typeIn.i;
}

TEST_F(AutoFilterFunctionalTest, FunctionDecorationTest) {
  // AddRecipient that is an instance of std::function f : a -> b
  // This must be satisfied by decoration of type a,
  // independent of the order of decoration.

  AutoRequired<AutoPacketFactory> factory;

  //Decoration with data first
  {
    auto packet = factory->NewPacket();
    packet->Decorate(Decoration<0>());
    packet->AddRecipient(AutoFilterDescriptor(&FilterFunction));
    const Decoration<1>* getdec;
    ASSERT_TRUE(packet->Get(getdec)) << "Decoration function was not called";
  }

  //Decoration with function first
  //NOTE: This test also catches failures to flush temporary subscriber information
  {
    auto packet = factory->NewPacket();
    packet->AddRecipient(AutoFilterDescriptor(&FilterFunction));
    packet->Decorate(Decoration<0>());
    const Decoration<1>* getdec;
    ASSERT_TRUE(packet->Get(getdec)) << "Decoration function was not called";
  }
}

TEST_F(AutoFilterFunctionalTest, FunctionDecorationLambdaTest) {
  AutoRequired<AutoPacketFactory> factory;

  //Decoration with function first
  {
    auto packet = factory->NewPacket();
    int addType = 1;
    auto sentry = std::make_shared<bool>(true);
    *packet +=
      [addType, sentry](const Decoration<0>& typeIn, auto_out<Decoration<1>> typeOut) {
        typeOut->i += 1 + typeIn.i;
      };

    // Sentry's use count should be precisely two at this point
    ASSERT_EQ(2UL, sentry.use_count()) << "Appended recipient lambda was destroyed unexpectedly when it should have been pended";

    packet->Decorate(Decoration<0>());
    const Decoration<1>* getdec;
    ASSERT_TRUE(packet->Get(getdec)) << "Decoration function was not called";
    ASSERT_EQ(1 + addType, getdec->i) << "Increment was not applied";
  }
}

TEST_F(AutoFilterFunctionalTest, FunctionInjectorTest) {
  AutoRequired<AutoPacketFactory> factory;

  auto packet = factory->NewPacket();
  int addType = 1;
  packet->AddRecipient([addType](auto_out<Decoration<0>> typeOut) {
    typeOut->i += addType;
  });
  const Decoration<0>* getdec;
  ASSERT_TRUE(packet->Get(getdec)) << "Decoration function was not called";
  ASSERT_EQ(0 + addType, getdec->i) << "Increment was not applied";
}

TEST_F(AutoFilterFunctionalTest, FunctionExtractorTest) {
  AutoRequired<AutoPacketFactory> factory;

  auto packet = factory->NewPacket();
  int extType = -1;
  packet->AddRecipient([&extType](const Decoration<1>& typeIn) {
    extType = typeIn.i;
  });
  packet->Decorate(Decoration<1>());
  ASSERT_EQ(1, extType) << "Decoration type was not extracted";
}

TEST_F(AutoFilterFunctionalTest, RecipientRemovalTest) {
  auto called = std::make_shared<bool>(false);
  AutoRequired<AutoPacketFactory> factory;

  // Add a recipient and then remove it, verify it doesn't get called
  auto packet = factory->NewPacket();
  AutoPacket::Recipient recipient =
    (
      *packet += [called] (const Decoration<0>&) {
        *called = true;
      }
    );
  packet->RemoveRecipient(std::move(recipient));

  ASSERT_FALSE(*called) << "A recipient that should have been removed was called";
}