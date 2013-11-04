#include "stdafx.h"
#include "DecoratorTest.h"
#include "FilterPropertyExtractor.h"

using namespace std;

/// <summary>
/// A simple "decoration" class
/// </summary>
template<int i>
class Decoration {
public:
  Decoration(void) :
    i(i)
  {}

  int i;
};

class DecoratorA {
public:
  void Filter(Decoration<0> zero, Decoration<1> one);
};

TEST_F(DecoratorTest, VerifyCorrectExtraction) {
  vector<const type_info*> v;

  // Run our prop extractor based on a known decorator:
  RecipientPropertyExtractor<DecoratorA>::Enumerate(
    [&v] (const std::type_info& ti) {
      v.push_back(&ti);
    }
  );

  ASSERT_EQ(2, v.size()) << "Extracted an insufficient number of types from a known filter function";

  // Arguments MUST be in order:
  EXPECT_EQ(typeid(Decoration<0>), *v[0]);
  EXPECT_EQ(typeid(Decoration<1>), *v[1]);
}

TEST_F(DecoratorTest, VerifyEmptyExtraction) {
  vector<const type_info*> v = RecipientPropertyExtractor<Object>::Enumerate();
  EXPECT_TRUE(v.empty()) << "Extracted arguments from an object known not to have a Filter method";
}

TEST_F(DecoratorTest, VerifySimpleRecipient) {

}