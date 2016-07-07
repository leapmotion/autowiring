// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/optional.h>

class OptionalTest :
  public testing::Test
{};

TEST_F(OptionalTest, PrimitiveCheck) {
  optional<int> x;
  ASSERT_FALSE(x);
  x = 10;
  ASSERT_TRUE(x);
  x = 0;
  ASSERT_TRUE(x);
  ASSERT_EQ((void*)&x, (void*)&x.value()) << "Expected value to be at the top of optional";
  x = {};
  ASSERT_FALSE(x);
}

TEST_F(OptionalTest, DestructionCheck) {
  auto x = std::make_shared<int>(100);
  {
    optional<std::shared_ptr<int>> y = x;
    ASSERT_EQ(2L, x.use_count());
    y = {};
    ASSERT_TRUE(x.unique());
  }
  ASSERT_TRUE(x.unique());
}

TEST_F(OptionalTest, MoveCheck) {
  std::unique_ptr<int> u{ new int{55} };

  optional<std::unique_ptr<int>> v = std::move(u);
  ASSERT_EQ(nullptr, u);
  ASSERT_EQ(55, **v);
}
