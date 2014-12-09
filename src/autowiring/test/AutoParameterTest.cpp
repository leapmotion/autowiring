// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/AutoParameter.h>
#include <limits.h>

class AutoParameterTest:
  public testing::Test
{};


struct MyParamClass1 {
  struct MyIntParam1 {
    static constexpr int Default() { return 15; }
  };
  
  AutoParameter<int, MyIntParam1> m_param;
};

TEST_F(AutoParameterTest, VerifyCorrectDeconstruction) {
  AutoRequired<MyParamClass1> mpc;
  auto& param = mpc->m_param;
  
  EXPECT_STREQ("AutoParam.MyParamClass1::MyIntParam1", param.m_key.c_str())
    << "Configuration variable name was not correctly extracted";
}

TEST_F(AutoParameterTest, VerifyDefaultValue) {
  AutoRequired<MyParamClass1> mpc;
  auto& param = mpc->m_param;
  
  ASSERT_EQ(*param, 15)
    << "Default value was not properly set";
  ASSERT_FALSE(param.IsConfigured())
    << "Using the default value does not mean the parameter should be configured/set";
}

TEST_F(AutoParameterTest, VerifySetShouldCallConfigure) {
  AutoRequired<MyParamClass1> mpc;
  auto& param = mpc->m_param;
  
  ASSERT_TRUE(param.Set(MyParamClass1::MyIntParam1::Default()) && param.IsConfigured())
    << "Settung the variable should configure it in the auto config manager";
}

TEST_F(AutoParameterTest, VerifyResetToDefaultValue) {
  AutoRequired<MyParamClass1> mpc;
  auto& param = mpc->m_param;

  ASSERT_TRUE(param.Set(30) && *param == 30)
    << "Could not set the parameter to another value";
  
  // Reset
  param.Set(MyParamClass1::MyIntParam1::Default());
  
  ASSERT_EQ(*param, 15)
    << "Parameter was not properly reset to its default value";
}


struct MyParamClass2 {
  struct MyIntParam2 {
    static constexpr int Default() { return 15; }
    static bool Validate(const int& value) { return 10 <= value && value <= 20; }
  };
  
  AutoParameter<int, MyIntParam2> m_param;
};

TEST_F(AutoParameterTest, VerifyValidationFunction) {
  AutoRequired<MyParamClass2> mpc;
  auto& param = mpc->m_param;
  
  ASSERT_FALSE(param.Set(9))
    << "Set() should return false when setting invalid value";
  ASSERT_EQ(*param, 15)
    << "Failed set attempts should not have altered the previous state";
  
  ASSERT_TRUE(param.Set(10) && *param == 10)
    << "Should be able to set values that are valid according to the validation function";
}


struct MyParamClass4 {
  struct MyIntParam4 {
    static constexpr int Default() { return 0; }
    static bool Validate(const int& value) { return 10 <= value && value <= 20; }
  };
  
  AutoParameter<int, MyIntParam4> m_param;
};

TEST_F(AutoParameterTest, VerifyInvalidDefaultValue) {
  ASSERT_ANY_THROW(AutoRequired<MyParamClass4>())
    << "Cannot construct a parameter where default value is invalid";
}
