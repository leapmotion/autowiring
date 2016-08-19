// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/marshaller.h>

class MarshallerTest:
  public testing::Test
{};

//TEMPORARILY DISABLED - Floating point marshaling is a hard problem and should
//be handled by a library. See Dragon4 and Grisu3, or https://github.com/google/double-conversion.

/*TEST_F(MarshallerTest, FloatTest) {
  autowiring::marshaller<float> b;

  float x = 0;
  std::string valX = b.marshal(&x);
  ASSERT_STREQ("0", valX.c_str());

  float outX = 20;
  b.unmarshal(&outX, valX.c_str());
  ASSERT_EQ(outX, x);

  float y = 0.0001f;
  std::string valY = b.marshal(&y);
  ASSERT_STREQ("0.0001", valY.c_str());

  float z = 1.23e10f;
  std::string valZ = b.marshal(&z);
  ASSERT_STREQ("12300000000", valZ.c_str());
}

TEST_F(MarshallerTest, DoubleTest) {
  autowiring::marshaller<double> b;

  double x = -0.00899999999999999;
  std::string val = b.marshal(&x);
  ASSERT_STREQ("-0.00899999999999999", val.c_str());
}
*/
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

  unsigned int y = std::numeric_limits<uint32_t>::max();
  std::string valY = b.marshal(&y);
  ASSERT_STREQ("4294967295", valY.c_str());

  unsigned int z = 20;
  std::string valZ = b.marshal(&z);

  unsigned int outZ = 0;
  b.unmarshal(&outZ, valZ.c_str());

  ASSERT_EQ(outZ, z);
}
