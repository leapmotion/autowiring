#include "stdafx.h"
#include "DecoratorTest.h"
#include "AutoPacket.h"
#include "AutoPacketFactory.h"
#include "FilterPropertyExtractor.h"

using namespace std;

/// <summary>
/// A simple "decoration" class which will be added to a variety of sample packets
/// </summary>
template<int i>
class Decoration {
public:
  Decoration(void) :
    i(i)
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
  auto vCopy = RecipientPropertyExtractor<FilterA>::Enumerate();
  EXPECT_EQ(v, vCopy) << "Two overloads of Enumerate returned contradictory types";
}

TEST_F(DecoratorTest, VerifyEmptyExtraction) {
  vector<const type_info*> v = RecipientPropertyExtractor<Object>::Enumerate();
  EXPECT_TRUE(v.empty()) << "Extracted arguments from an object known not to have a Filter method";
}

TEST_F(DecoratorTest, VerifySimpleFilter) {
  AutoRequired<FilterA> filterA;
  AutoRequired<AutoPacketFactory> factory;

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