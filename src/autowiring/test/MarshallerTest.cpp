// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/marshaller.h>

class MarshallerTest:
  public testing::Test
{};

TEST_F(MarshallerTest, DoubleMarshalTest) {
  autowiring::builtin_marshaller<double, void> b;

  double x = -0.00899999999999999;
  std::string val = b.marshal(&x);
  ASSERT_STREQ("-0.00899999999999999", val.c_str());
}
