#include "stdafx.h"
#include "DecoratorTest.h"
#include "AutoPacket.h"
#include "AutoPacketFactory.h"
#include "FilterPropertyExtractor.h"
#include "TestFixtures/Decoration.h"

using namespace std;

DecoratorTest::DecoratorTest(void) {
  // All decorator tests must run from an initiated context
  AutoCurrentContext()->Initiate();
}

TEST_F(DecoratorTest, VerifyCorrectExtraction) {
  vector<const type_info*> v;

  // Run our prop extractor based on a known decorator:
  RecipientPropertyExtractor<FilterA>::Enumerate(
    [&v] (const std::type_info& ti) {
      v.push_back(&ti);
    }
  );
  ASSERT_EQ(2UL, v.size()) << "Extracted an insufficient number of types from a known filter function";

  // Arguments MUST be in order:
  EXPECT_EQ(typeid(Decoration<0>), *v[0]);
  EXPECT_EQ(typeid(Decoration<1>), *v[1]);

  // Verify both overloads wind up returning the same array:
  auto ppCur = RecipientPropertyExtractor<FilterA>::Enumerate();
  for(size_t i = 0; ppCur[i]; i++)
    EXPECT_EQ(*v[i], *ppCur[i]) << "Two overloads of Enumerate returned contradictory types";
}

TEST_F(DecoratorTest, VerifyEmptyExtraction) {
  const type_info*const* v = RecipientPropertyExtractor<Object>::Enumerate();
  EXPECT_EQ(nullptr, *v) << "Extracted arguments from an object known not to have a Filter method";
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
  EXPECT_EQ(&knownDec0, &dec0) << "Decoration 0 returned at an incorrect location";
  EXPECT_EQ(&knownDec1, &dec1) << "Decoration 1 returned at an incorrect location";
  EXPECT_EQ(&knownDec2, &dec2) << "Decoration 2 returned at an incorrect location";

  // Verify content correctness:
  EXPECT_EQ(0, dec0.i) << "Decoration 0 incorrectly persisted";
  EXPECT_EQ(1, dec1.i) << "Decoration 1 incorrectly persisted";
  EXPECT_EQ(2, dec2.i) << "Decoration 2 incorrectly persisted";
}

TEST_F(DecoratorTest, VerifyDecoratorAwareness) {
  // Create a packet while the factory has no subscribers:
  AutoRequired<AutoPacketFactory> factory;
  auto packet1 = factory->NewPacket();

  // Verify subscription-free status:
  EXPECT_FALSE(packet1->HasSubscribers<Decoration<0>>()) << "Subscription exists where one should not have existed";

  // Create another packet where a subscriber exists:
  AutoRequired<FilterA> filterA;
  auto packet2 = factory->NewPacket();

  // Verify the first packet still does not have subscriptions:
  EXPECT_FALSE(packet1->HasSubscribers<Decoration<0>>()) << "Subscription was incorrectly, retroactively added to a packet";

  // Verify the second one does:
  EXPECT_TRUE(packet2->HasSubscribers<Decoration<0>>()) << "Packet lacked an expected subscription";
}