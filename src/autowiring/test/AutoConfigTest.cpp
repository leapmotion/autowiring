// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/config_descriptor.h>
#include <autowiring/config.h>
#include <autowiring/ConfigRegistry.h>
#include <autowiring/observable.h>

class AutoConfigTest:
  public testing::Test
{};

namespace {
  class MyConfigurableClass {
  public:
    autowiring::config<std::string> a{ "Hello world!" };
    std::atomic<int> b{ 929 };
    std::atomic<unsigned int> bUnsigned{ 92999 };
    volatile bool c = false;
    volatile float d = 1.0f;
    volatile double e = 99.2;
    autowiring::observable<int> obs{ 44 };
    autowiring::config<int> cfg{ 929999 };

    static autowiring::config_descriptor GetConfigDescriptor(void) {
      return {
        { "a", "Field A description", &MyConfigurableClass::a, "Hello world!" },
        { "b", "Field B description", &MyConfigurableClass::b, 929 },
        { "bUnsigned", &MyConfigurableClass::bUnsigned },
        { "c", &MyConfigurableClass::c },
        { "d", &MyConfigurableClass::d },
        { "e", &MyConfigurableClass::e },
        { "obs", &MyConfigurableClass::obs },
        { "cfg", &MyConfigurableClass::cfg }
      };
    }
  };

  class BadClass {
  public:
    autowiring::config_descriptor GetConfigDescriptor(void) { return{}; }
  };
}

static_assert(autowiring::has_getconfigdescriptor<MyConfigurableClass>::value, "Static new not correctly detected");
static_assert(!autowiring::has_getconfigdescriptor<BadClass>::value, "Bad class cannot have a configuration descriptor");

TEST_F(AutoConfigTest, ConfigFieldAssign) {
  autowiring::config<std::string> x{ "Hello world!" };
  ASSERT_TRUE(x.is_dirty()) << "Config values are assumed to be initially dirty";
}

TEST_F(AutoConfigTest, String) {
  MyConfigurableClass c;

  autowiring::config_field cf("a", "Field A description", &MyConfigurableClass::a, std::string{ "Hello world!" });

  std::string expected{ "Forescore and seven years ago" };
  autowiring::ConfigSet("a", c, expected.c_str());
  ASSERT_TRUE(c.a.clear_dirty());
  ASSERT_STREQ(expected.c_str(), c.a->c_str()) << "String configuration value not assigned";
}

TEST_F(AutoConfigTest, Integer) {
  MyConfigurableClass c;

  autowiring::ConfigSet("b", c, "999");
  ASSERT_EQ(c.b, 999) << "Integer configuration value not assigned";

  autowiring::ConfigSet("b", c, "-999");
  ASSERT_EQ(c.b, -999) << "Negative integer configuration value not assigned";
}

TEST_F(AutoConfigTest, IntegerUnsigned) {
  MyConfigurableClass c;

  c.bUnsigned = 10929;
  std::string strVal = autowiring::ConfigGet("bUnsigned", c);
  ASSERT_STREQ("10929", strVal.c_str());

  autowiring::ConfigSet("bUnsigned", c, "999");
  ASSERT_EQ(c.bUnsigned, 999) << "Integer configuration value not assigned";
  ASSERT_ANY_THROW(autowiring::ConfigSet("bUnsigned", c, "-999")) << "Incorrectly assigned as signed value to an unsigned field";
}

TEST_F(AutoConfigTest, Bool) {
  MyConfigurableClass c;

  autowiring::ConfigSet("c", c, "true");
  ASSERT_TRUE(c.c) << "Boolean configuration value not assigned";
}

TEST_F(AutoConfigTest, Float) {
  MyConfigurableClass c;

  c.d = 10929.4f;
  std::string strVal = autowiring::ConfigGet("d", c);
  ASSERT_STREQ("10929.4", strVal.c_str());

  autowiring::ConfigSet("d", c, "123.4");
  ASSERT_FLOAT_EQ(c.d, 123.4f) << "Float configuration value not assigned";
}

TEST_F(AutoConfigTest, Double) {
  MyConfigurableClass c;
  std::string strVal;

  c.e = 10929.4423;
  strVal = autowiring::ConfigGet("e", c);
  ASSERT_STREQ("10929.4423", strVal.c_str());

  c.e = 109290000000000;
  strVal = autowiring::ConfigGet("e", c);
  ASSERT_STREQ("109290000000000", strVal.c_str());

  c.e = -0.0099291;
  strVal = autowiring::ConfigGet("e", c);
  ASSERT_STREQ("-0.0099291", strVal.c_str());

  autowiring::ConfigSet("e", c, "77482.4");
  ASSERT_DOUBLE_EQ(c.e, 77482.4) << "Double configuration value not assigned";
}

TEST_F(AutoConfigTest, Observable) {
  MyConfigurableClass c;

  bool hit = false;
  c.obs.onChanged += [&hit] { hit = true; };
  autowiring::ConfigSet("obs", c, "101");
  ASSERT_EQ(101, c.obs) << "Value not properly assigned";
  ASSERT_TRUE(hit) << "Observable signal not asserted";
}

TEST_F(AutoConfigTest, Configurable) {
  AutoCurrentContext ctxt;
  AutoRequired<MyConfigurableClass> c;

  ASSERT_TRUE(c->cfg.clear_dirty()) << "Dirty flag was not set on a value that should have been dirty";
  ASSERT_FALSE(c->cfg.is_dirty());
  ctxt->ConfigSet("cfg", "888");
  ASSERT_TRUE(c->cfg.is_dirty()) << "Dirty flag not set correctly after being updated in configuration";
  ASSERT_TRUE(c->cfg.clear_dirty());
  ASSERT_EQ(888, c->cfg);
}

TEST_F(AutoConfigTest, ContextSetSimple) {
  AutoCurrentContext ctxt;
  AutoRequired<MyConfigurableClass> mcc;

  ctxt->ConfigSet("b", "1029");
  ASSERT_EQ(mcc->b, 1029);
}

TEST_F(AutoConfigTest, ContextSetAfter) {
  AutoCurrentContext ctxt;
  ctxt->ConfigSet("b", "10442");

  AutoRequired<MyConfigurableClass> mcc;
  ASSERT_EQ(mcc->b, 10442);
}
