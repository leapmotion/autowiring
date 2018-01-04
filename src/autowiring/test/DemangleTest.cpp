// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/demangle.h>

class DemangleTest:
  public testing::Test
{};

class DemangleTestType {};

TEST_F(DemangleTest, DemanglePreciseResultSimple) {
  ASSERT_STREQ(
    "DemangleTestType",
    autowiring::demangle(typeid(DemangleTestType)).c_str()
  ) << "Mismatch on a simple non-template type";
}

template<typename T>
class TemplatedDemangleTestType {};

TEST_F(DemangleTest, DemangleSingleTemplate) {
  ASSERT_STREQ(
    "TemplatedDemangleTestType<DemangleTestType>",
    autowiring::demangle(typeid(TemplatedDemangleTestType<DemangleTestType>)).c_str()
  ) << "Single templated type was not demangled correctly";
}

template<typename T, typename U>
class TwoArgTemplatedDemangleTestType {};

TEST_F(DemangleTest, DemangleDoubleTemplate) {
  ASSERT_STREQ(
    "TwoArgTemplatedDemangleTestType<DemangleTestType, DemangleTestType>",
    autowiring::demangle(
      typeid(TwoArgTemplatedDemangleTestType<DemangleTestType, DemangleTestType>)
    ).c_str()
  ) << "Single templated type was not demangled correctly";
}
