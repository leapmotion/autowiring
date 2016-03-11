// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/tweak.h>
#include <gtest/gtest.h>

namespace aw = autowiring;

TEST(TweakTest, Initial) {
  autowiring::tweak<int> x;
  ASSERT_EQ(0, x) << "Tweak value was supposed to be zero-initialized";
  ASSERT_FALSE(x.is_dirty()) << "Value should have been initially dirty";
}

TEST(TweakTest, Dirty) {
  autowiring::tweak<int> x;
  x = 10;
  ASSERT_TRUE(x.is_dirty());
  ASSERT_TRUE(x.clear_dirty());
}

namespace {
  class SimpleStruct {
  public:
    autowiring::tweak<int> v;

    autowiring::config_descriptor GetDescriptor(void) {
      return {
        { "Abcd", &SimpleStruct::v }
      };
    }
  };
}

TEST(TweakTest, Value) {
  aw::marshaller<aw::tweak<int>> marshaller;
  aw::tweak<int> value = 787;
  std::string v = marshaller.marshal(&value);
  ASSERT_STREQ("787", v.c_str());
}
