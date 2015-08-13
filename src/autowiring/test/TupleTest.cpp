// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/auto_tuple.h>
#include <string>

class TupleTest:
  public testing::Test
{};

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

  ASSERT_EQ(&tup.value, &v) << "Get operation did not provide a correct reference to the underlying tuple value";
}
