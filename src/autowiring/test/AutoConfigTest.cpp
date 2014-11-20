// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/AutoConfig.h>

class AutoConfigTest:
  public testing::Test
{};

struct MyConfigurableClass {
  AutoConfig<int, struct XYZ> m_myName;
};

TEST_F(AutoConfigTest, VerifyCorrectDeconstruction) {
  MyConfigurableClass mcc;

  EXPECT_STREQ("MyConfigurableClass", mcc.m_myName.Class.c_str()) << "Configuration variable enclosing class was not correctly extracted";
  EXPECT_STREQ("XYZ", mcc.m_myName.Name.c_str()) << "Configuration variable name was not correctly extracted";
}