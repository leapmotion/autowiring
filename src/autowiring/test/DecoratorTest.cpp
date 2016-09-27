// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"
#include <autowiring/AutoPacket.h>
#include <autowiring/AutoPacketFactory.h>
#include <autowiring/SatCounter.h>

using namespace std;

class DecoratorTest:
  public testing::Test
{
public:
  DecoratorTest(void) {
    // All decorator tests must run from an initiated context
    AutoCurrentContext()->Initiate();
  }
};

TEST_F(DecoratorTest, VerifyCorrectExtraction) {
  vector<auto_id> v;

  // Run our prop extractor based on a known decorator:
  AutoRequired<FilterA> filterA;
  AutoFilterDescriptor desc(static_cast<std::shared_ptr<FilterA>&>(filterA));
  for(const AutoFilterArgument* cur = desc.GetAutoFilterArguments(); *cur; cur++)
    v.push_back(cur->id);
  ASSERT_EQ(2UL, v.size()) << "Extracted an insufficient number of types from a known filter function";

  // Arguments MUST be in order:
  ASSERT_EQ(auto_id_t<Decoration<0>>{}, v[0]);
  ASSERT_EQ(auto_id_t<Decoration<1>>{}, v[1]);
}

TEST_F(DecoratorTest, VerifyEmptyExtraction) {
  auto obj = std::make_shared<CoreObject>();

  // Should be possible to obtain this value and have it remain valid even after the descriptor is gone
  const AutoFilterArgument* v = MakeAutoFilterDescriptor(obj).GetAutoFilterArguments();
  ASSERT_EQ(nullptr, v) << "Extracted arguments from an object known not to have a Filter method";
}

TEST_F(DecoratorTest, VerifySimplePacketDecoration) {
  AutoRequired<AutoPacketFactory> factory;

  // Create the packet we will be persisting:
  auto packet = factory->NewPacket();

  // Add a few decorations on this packet:
  auto& knownDec0 = packet->Decorate(Decoration<0>());
  auto& knownDec1 = packet->Decorate(Decoration<1>());
  auto& knownDec2 = packet->Decorate(Decoration<2>());

  // Verify we can get these packets back--might throw exceptions here!
  auto& dec0 = packet->Get<Decoration<0>>();
  auto& dec1 = packet->Get<Decoration<1>>();
  auto& dec2 = packet->Get<Decoration<2>>();

  // Verify identities:
  ASSERT_EQ(&knownDec0, &dec0) << "Decoration 0 returned at an incorrect location";
  ASSERT_EQ(&knownDec1, &dec1) << "Decoration 1 returned at an incorrect location";
  ASSERT_EQ(&knownDec2, &dec2) << "Decoration 2 returned at an incorrect location";

  // Verify content correctness:
  ASSERT_EQ(0, dec0.i) << "Decoration 0 incorrectly persisted";
  ASSERT_EQ(1, dec1.i) << "Decoration 1 incorrectly persisted";
  ASSERT_EQ(2, dec2.i) << "Decoration 2 incorrectly persisted";
}

TEST_F(DecoratorTest, VerifyDecoratorAwareness) {
  auto filter = [](const Decoration<0>& zero, const Decoration<1>& one) {};

  // Create a packet while the factory has no subscribers:
  AutoRequired<AutoPacketFactory> factory;
  auto packet1 = factory->NewPacket();

  // Verify subscription-free status:
  ASSERT_FALSE(packet1->HasSubscribers<Decoration<0>>()) << "Subscription exists where one should not have existed";

  // Create another packet where a subscriber exists:
  *factory += filter;
  auto packet2 = factory->NewPacket();

  // Verify the first packet still does not have subscriptions:
  ASSERT_THROW(packet1->GetSatisfaction<decltype(filter)>(), autowiring_error) << "Subscription was incorrectly, retroactively added to a packet";
  ASSERT_FALSE(packet1->HasSubscribers<Decoration<0>>()) << "Subscription was incorrectly, retroactively added to a packet";
  ASSERT_EQ(0UL, packet1->GetDecorationTypeCount()) << "Subscription was incorrectly, retroactively added to a packet";
  ASSERT_FALSE(packet1->HasSubscribers<Decoration<0>>()) << "Subscription was incorrectly, retroactively added to a packet";

  // Verify the second one does:
  ASSERT_NO_THROW(packet2->GetSatisfaction<decltype(filter)>()) << "Packet lacked an expected subscription";
  ASSERT_EQ(2UL, packet2->GetDecorationTypeCount()) << "Incorrect count of expected decorations";
  ASSERT_TRUE(packet2->HasSubscribers<Decoration<0>>()) << "Packet lacked an expected subscription";
}

TEST_F(DecoratorTest, ForwardAllTest) {
  AutoRequired<AutoPacketFactory> factory;
  auto packet1 = factory->NewPacket();
  auto packet2 = factory->NewPacket();

  packet1->Decorate(Decoration<0>());
  packet1->ForwardAll(packet2);

  ASSERT_TRUE(packet2->Has<Decoration<0>>()) << "Forwarded packet did not have a decoration present on the original packet as expected";
}
