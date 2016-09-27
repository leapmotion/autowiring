// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/uuid.h>

class UuidTest:
  public testing::Test
{};

DECLARE_UUID(MyX, "01234567-89AB-CDEF-FEDC-BA9876543210"){};

TEST_F(UuidTest, VerifySimpleUuid) {
  // Verify table properties:
  ASSERT_EQ(0xE, UUID_MAPPING_TABLE['e' - '0']) << "Hex character 'e' in the mapping table was not correctly mapped";
  ASSERT_EQ(0xA, UUID_MAPPING_TABLE['A' - '0']) << "Hex character 'A' in the mapping table was not correctly mapped";

  // Verify the queried UUID:
  uuid test_uuid = uuid_of<MyX>::Uuid();

  ASSERT_EQ(0x1234567UL, test_uuid.Data1) << "Data1 was not defined properly";

  ASSERT_EQ(0x89ABUL, test_uuid.Data2) << "Data2 was not defined properly";
  ASSERT_EQ(0xCDEFUL, test_uuid.Data3) << "Data3 was not defined properly";
  ASSERT_EQ(0x1032547698badcfe, (long long&) test_uuid.Data4) << "Data4 was not defined properly";
}

TEST_F(UuidTest, VerifyEquivalence) {
  const uuid one("{5D0060CA-16CB-4E75-89A8-8ED13AA15672}");
  const uuid two("5D0060CA-16CB-4E75-89A8-8ED13AA15672");

  ASSERT_EQ(one, two) << "The two acceptable formats for a UUID did not evaluate to equivalence";
}
