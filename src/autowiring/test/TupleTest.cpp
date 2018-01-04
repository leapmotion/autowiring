// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/auto_tuple.h>
#include <string>

static_assert(10 == autowiring::sum<1, 2, 7>::value, "Sum template function did not evaluate a sum correctly");
static_assert(autowiring::is_tuple<autowiring::tuple<int, int>>::value, "Autowiring tuple not correctly recognized as a tuple");

class TupleTest:
  public testing::Test
{};

static_assert(
  autowiring::find<long, int, long, float>::value,
  "Value known to be present in a tuple was not found as expected"
);

static_assert(
  3 == autowiring::find<float, int, long, float>::index,
  "Known-present value was found at the wrong index"
);

static_assert(
  1 == autowiring::find<int, int, long, float>::index,
  "Known-present value was found at the wrong index"
);

static_assert(
  0 == autowiring::find<double, int, long, float>::index,
  "Known-absent value was incorrectly reported as being present"
);

TEST_F(TupleTest, CallTest) {
  autowiring::tuple<int, int, std::string> t(101, 102, "Hello world!");

  ASSERT_EQ(101, autowiring::get<0>(t)) << "First tuple value was invalid";
  ASSERT_EQ(102, autowiring::get<1>(t)) << "Second tuple value was invalid";
}

TEST_F(TupleTest, TieTest) {
  std::unique_ptr<int> val(new int(22));

  autowiring::tuple<std::unique_ptr<int>&> tup(val);
  ASSERT_EQ(22, *autowiring::get<0>(tup)) << "Tied tuple did not retrieve the expected value";
}

TEST_F(TupleTest, AddressTest) {
  autowiring::tuple<std::unique_ptr<int>> tup;
  std::unique_ptr<int>& v = autowiring::get<0>(tup);

  ASSERT_EQ(&tup.val, &v) << "Get operation did not provide a correct reference to the underlying tuple value";
}

TEST_F(TupleTest, GetByTypeTest) {
  autowiring::tuple<int, long, float> tup{ 1, 2, 1.9f };
  int& iVal = autowiring::get<int>(tup);
  long& lVal = autowiring::get<long>(tup);
  float& fVal = autowiring::get<float>(tup);

  ASSERT_EQ(1, iVal) << "Integer value type mismatch";
  ASSERT_EQ(2, lVal) << "Long value type mismatch";
  ASSERT_EQ(1.9f, fVal) << "Float value type mismatch";
}

namespace {
  class CountsCopies {
  public:
    CountsCopies(void) = default;
    CountsCopies(const CountsCopies& rhs) { nCopies++; }
    void operator=(const CountsCopies& rhs) { nCopies++; }
    static int nCopies;
  };

  int CountsCopies::nCopies = 0;
}

TEST_F(TupleTest, NoUnneededCopies) {
  CountsCopies::nCopies = 0;

  CountsCopies isCopied;
  autowiring::tuple<CountsCopies> nCopies(isCopied);

  ASSERT_EQ(1, CountsCopies::nCopies) << "Too many copies made during in-place tuple construction";
}

TEST_F(TupleTest, CanHoldMoveOnly) {
  auto up = std::unique_ptr<int>{ new int {999} };
  int* pVal = up.get();

  autowiring::tuple<std::unique_ptr<int>, bool> pp{ std::move(up), false };
  autowiring::tuple<std::unique_ptr<int>, bool> rhs = std::move(pp);

  ASSERT_EQ(nullptr, autowiring::get<0>(pp).get()) << "Tuple move did not wipe out an r-value properly";
  ASSERT_EQ(pVal, autowiring::get<0>(rhs).get()) << "Tuple move did not transfer an r-value properly";

  rhs = std::move(rhs);
  ASSERT_EQ(pVal, autowiring::get<0>(rhs).get()) << "Reflexive transfer invalidated incorrectly";
}
