// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
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
  *typeOut = Decoration<1>(1 + 1 + typeIn.i);
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
    ASSERT_EQ(1, packet->GetDecorationTypeCount()) << "Unexpected number of decorations";
    packet->AddRecipient(AutoFilterDescriptor(&FilterFunction));
    const Decoration<1>* getdec;
    ASSERT_TRUE(packet->Get(getdec)) << "Decoration function was not called";
    ASSERT_EQ(2, packet->GetDecorationTypeCount()) << "Unexpected number of decorations";
  }

  //Decoration with function first
  //NOTE: This test also catches failures to flush temporary subscriber information
  {
    auto packet = factory->NewPacket();
    packet->AddRecipient(AutoFilterDescriptor(&FilterFunction));
    packet->Decorate(Decoration<0>());
    const Decoration<1>* getdec;
    ASSERT_TRUE(packet->Get(getdec)) << "Decoration function was not called";
    ASSERT_EQ(2, packet->GetDecorationTypeCount()) << "Unexpected number of decorations";
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
        *typeOut = Decoration<1>(1 + 1 + typeIn.i);
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
    *typeOut = Decoration<0>(addType);
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
  const auto recipient =
    (
      *packet += [called] (const Decoration<0>&) {
        *called = true;
      }
    );
  packet->RemoveRecipient(*recipient);

  ASSERT_FALSE(*called) << "A recipient that should have been removed was called";
}

TEST_F(AutoFilterFunctionalTest, ObservingFunctionTest) {
  AutoRequired<AutoPacketFactory> factory;

  auto packet = factory->NewPacket();

  // Attach a lambda to a const version of this packet:
  auto called = std::make_shared<bool>(false);
  static_cast<const AutoPacket&>(*packet) += [called](const Decoration<0>& dec, Decoration<1> dec1) {
    *called = true;
  };

  // Verify that a call was made as expected once we decorate:
  packet->Decorate(Decoration<0>());
  packet->Decorate(Decoration<1>());
  ASSERT_TRUE(*called) << "Receive-only filter attached to a const packet image was not correctly called";
}

TEST_F(AutoFilterFunctionalTest, TripleFunctionTest) {
  AutoRequired<AutoPacketFactory> factory;

  auto packet = factory->NewPacket();

  // Attach two lambdas, verify they each get called when we expect;
  auto called1 = std::make_shared<bool>(false);
  auto called2 = std::make_shared<bool>(false);
  auto called3 = std::make_shared<bool>(false);
  *packet += [called1](const Decoration<0>&) { *called1 = true; };
  *packet += [called2](const Decoration<0>&, const Decoration<1>&) { *called2 = true; };
  *packet += [called3](const Decoration<1>&) { *called3 = true; };

  // Only the third method should be called
  packet->Decorate(Decoration<1>());

  ASSERT_FALSE(*called1) << "Incorrect filter method called in response to Decorate";
  ASSERT_FALSE(*called2) << "Incorrect filter method called in response to Decorate";
  ASSERT_TRUE(*called3) << "Filter method called when a decoration was absent";

  // Now the other two should be called:
  packet->Decorate(Decoration<0>());
  ASSERT_TRUE(*called1) << "Final observer method was not invoked as expected";
  ASSERT_TRUE(*called2) << "Two-argument observer method not invoked as expected";
}

TEST_F(AutoFilterFunctionalTest, MultiPostHocIntroductionTest) {
  AutoCurrentContext()->Initiate();
  AutoRequired<AutoPacketFactory> factory;

  int called = 0;

  *factory += [&called](int& out) { out = called++; };
  *factory += [&called](int& out) { out = called++; };

  // Add a gather step on the packet:
  auto packet = factory->NewPacket();
  *packet += [&called](const int* vals []){
    ASSERT_NE(nullptr, vals);
    called++;
  };

  ASSERT_EQ(3, called) << "Not all lambda functions were called as expected";
}
