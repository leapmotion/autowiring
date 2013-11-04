#include "stdafx.h"
#include "DecoratorTest.h"
#include "AutoPacket.h"
#include "AutoPacketFactory.h"
#include "FilterPropertyExtractor.h"

using namespace std;

/// <summary>
/// A simple "decoration" class which will be added to a variety of sample packets
/// </summary>
template<int N>
class Decoration {
public:
  Decoration(void) :
    i(N)
  {}

  int i;
};

class FilterA {
public:
  FilterA(void) :
    m_called(false)
  {}

  void AutoFilter(Decoration<0> zero, Decoration<1> one) {
    m_called = true;
    m_zero = zero;
    m_one = one;
  }

  bool m_called;
  Decoration<0> m_zero;
  Decoration<1> m_one;
};
static_assert(has_autofilter<FilterA>::value, "Expected the filter to have an AutoFilter method");

TEST_F(DecoratorTest, VerifyCorrectExtraction) {
  vector<const type_info*> v;

  // Run our prop extractor based on a known decorator:
  RecipientPropertyExtractor<FilterA>::Enumerate(
    [&v] (const std::type_info& ti) {
      v.push_back(&ti);
    }
  );
  ASSERT_EQ(2, v.size()) << "Extracted an insufficient number of types from a known filter function";

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
  auto f = factory->NewPacket();

  // Add a few decorations on this packet:
  auto& knownDec0 = f->Decorate(Decoration<0>());
  auto& knownDec1 = f->Decorate(Decoration<1>());
  auto& knownDec2 = f->Decorate(Decoration<2>());

  // Verify we can get these packets back--might throw exceptions here!
  auto& dec0 = f->Get<Decoration<0>>();
  auto& dec1 = f->Get<Decoration<1>>();
  auto& dec2 = f->Get<Decoration<2>>();

  // Verify identities:
  EXPECT_EQ(&knownDec0, &dec0) << "Decoration 0 returned at an incorrect location";
  EXPECT_EQ(&knownDec1, &dec1) << "Decoration 1 returned at an incorrect location";
  EXPECT_EQ(&knownDec2, &dec2) << "Decoration 2 returned at an incorrect location";

  // Verify content correctness:
  EXPECT_EQ(0, dec0.i) << "Decoration 0 incorrectly persisted";
  EXPECT_EQ(1, dec1.i) << "Decoration 1 incorrectly persisted";
  EXPECT_EQ(2, dec2.i) << "Decoration 2 incorrectly persisted";
}

TEST_F(DecoratorTest, VerifySimpleFilter) {
  AutoRequired<FilterA> filterA;
  AutoRequired<AutoPacketFactory> factory;

  // Manually register the subscriber:
  factory->AddSubscriber(filterA);

  // Obtain a packet from the factory:
  auto f = factory->NewPacket();

  // Decorate with one instance:
  f->Decorate(Decoration<0>());

  // Verify that no hit takes place with inadequate decoration:
  EXPECT_FALSE(filterA->m_called) << "Filter called prematurely with insufficient decoration";

  // Now decorate with the other requirement of the filter:
  f->Decorate(Decoration<1>());

  // A hit should have taken place at this point:
  EXPECT_TRUE(filterA->m_called) << "Filter was not called even though it was fully satisfied";
}
