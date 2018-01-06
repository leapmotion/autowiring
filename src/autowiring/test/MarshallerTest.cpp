// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/marshaller.h>

class MarshallerTest:
  public testing::Test
{};

TEST_F(MarshallerTest, FloatTest) {
  autowiring::marshaller<float> f;

  float x = 0;
  std::string valX = f.marshal(&x);
  ASSERT_STREQ("0", valX.c_str()) << "Failed to properly output 0";

  float outX = 20;
  f.unmarshal(&outX, valX.c_str());
  ASSERT_EQ(outX, 0) << "Failed to properly round trip 0";

  float y = 0.0001f;
  std::string valY = f.marshal(&y);

  float outY = 0;
  f.unmarshal(&outY, valY.c_str());
  ASSERT_EQ(y, outY) << "Failed to properly round trip 0.0001f";

  float z = 1.23e8f;
  std::string valZ = f.marshal(&z);
  ASSERT_STREQ("123000000", valZ.c_str()) << "Failed to marshal a value with an exponent";
}

TEST_F(MarshallerTest, DoubleMarshalTest) {
  autowiring::marshaller<double> d;

  double x = -0.00899999999999999;
  std::string val = d.marshal(&x);
  double outX;
  d.unmarshal(&outX, val.c_str());
  ASSERT_EQ(x,outX) << "Failed to round trip a large double precision value";
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
