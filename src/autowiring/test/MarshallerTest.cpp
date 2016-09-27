// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/marshaller.h>

class MarshallerTest:
  public testing::Test
{};

TEST_F(MarshallerTest, DoubleMarshalTest) {
  autowiring::marshaller<double> b;

  double x = -0.00899999999999999;
  std::string val = b.marshal(&x);
  ASSERT_STREQ("-0.00899999999999999", val.c_str());
}

TEST_F(MarshallerTest, NegativeValues) {
  autowiring::marshaller<int> b;

  int x = -9;
  std::string valX = b.marshal(&x);
  ASSERT_STREQ("-9", valX.c_str());

  int y = 0xCDCDCDCD;
  std::string valY = b.marshal(&y);
  ASSERT_STREQ("-842150451", valY.c_str());

  int z = 0;
  std::string valZ = b.marshal(&z);
  ASSERT_STREQ("0", valZ.c_str());
}

TEST_F(MarshallerTest, UnsignedValues) {
  autowiring::marshaller<unsigned int> b;

  int x = 0;
  std::string valX = b.marshal(&x);
  ASSERT_STREQ("0", valX.c_str());
}

TEST_F(MarshallerTest, RoundTripTest) {
  autowiring::marshaller<unsigned int> b;

  unsigned int x = 20;
  std::string valX = b.marshal(&x);

  unsigned int outX = 0;
  b.unmarshal(&outX, valX.c_str());

  ASSERT_EQ(outX, x);
}
