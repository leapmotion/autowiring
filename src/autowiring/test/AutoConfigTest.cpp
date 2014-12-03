// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/AutoConfig.h>
#include <autowiring/AutoConfigManager.h>
#include <src/autowiring/AutoConfigParser.hpp>

class AutoConfigTest:
  public testing::Test
{};

struct MyConfigurableClass {
  AutoConfig<int, struct Namespace1, struct XYZ> m_myName;
};

struct MyConfigurableClass2 {
  AutoConfig<int, struct Namespace2, struct XYZ> m_myName;
};

TEST_F(AutoConfigTest, VerifyCorrectDeconstruction) {
  AutoRequired<MyConfigurableClass> mcc;

  EXPECT_STREQ("Namespace1.XYZ", mcc->m_myName.m_key.c_str())
    << "Configuration variable name was not correctly extracted";
}

TEST_F(AutoConfigTest, VerifySimpleAssignment) {
  // Set an attribute in the manager before injecting anything:
  AutoRequired<AutoConfigManager> acm;
  acm->Set("Namespace1.XYZ", 323);

  // Now inject the type which expects this value to be assigned:
  AutoRequired<MyConfigurableClass> mcc;
  ASSERT_EQ(323, mcc->m_myName) << "Configurable type did not receive a value as expected";
}

struct MyBoolClass {
  AutoConfig<bool, struct bool_space, struct my_bool> m_bool;
};

TEST_F(AutoConfigTest, VerifyBool) {
  AutoRequired<AutoConfigManager> acm;
  AutoRequired<MyBoolClass> clz1;
  
  acm->Set("bool_space.my_bool", true);
  ASSERT_TRUE(clz1->m_bool);
  
  acm->SetParsed("bool_space.my_bool", "false");
  ASSERT_FALSE(clz1->m_bool);
}

TEST_F(AutoConfigTest, VerifyPostHocAssignment) {
  // Inject the configurable type first
  AutoRequired<MyConfigurableClass> mcc;

  // Configuration manager must exist at this point as a consequence of the earlier construction
  Autowired<AutoConfigManager> acm;
  ASSERT_TRUE(acm.IsAutowired()) << "AutoConfig field did not inject a configuration manager into this context as expected";

  acm->Set("Namespace1.XYZ", 323);

  // Now inject the type which expects this value to be assigned:
  ASSERT_EQ(323, mcc->m_myName) << "Configurable type did not receive a value as expected";
}

TEST_F(AutoConfigTest, VerifyRecursiveSearch) {
  AutoRequired<AutoConfigManager> acm;
  acm->Set("Namespace1.XYZ", 1001);

  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);

    // Now inject an element here, and verify that it was wired up as expected:
    AutoRequired<MyConfigurableClass> mcc;
    ASSERT_TRUE(mcc->m_myName.IsConfigured()) << "A configurable value was not configured as expected";
    ASSERT_EQ(1001, mcc->m_myName) << "Configurable value obtained from a parent scope did not have the correct value";

    // This must work as expected--a local context override will rewrite configuration values in the local scope
    AutoRequired<AutoConfigManager> sub_mcc;
    sub_mcc->Set("Namespace1.XYZ", 1002);
    ASSERT_EQ(1002, mcc->m_myName) << "Override of a configurable value in a derived class did not take place as expected";
  }
}

struct DefaultName {
  AutoConfig<int, struct defaultname1> m_def;
};

TEST_F(AutoConfigTest, DefaultNamespace) {
  AutoRequired<AutoConfigManager> acm;
  acm->Set("defaultname1", 123);
  
  AutoRequired<DefaultName> def;
  
  ASSERT_EQ(123, def->m_def);
}

TEST_F(AutoConfigTest, VerifyParsedAssignment) {
  // We must also be able to support implicit string-to-type conversion via the shift operator for this type
  AutoRequired<AutoConfigManager> acm;

  // Direct assignment to a string should not work, the type isn't a string it's an int
  ASSERT_ANY_THROW(acm->Set("Namespace1.XYZ", "327")) << "An attempt to assign a value to an unrelated type did not generate an exception as expected";
  
  ASSERT_ANY_THROW(acm->Set("Namespace1.XYZ", 3.0)) << "An attempt to assign a value to an unrelated type did not generate an exception as expected";

  // Assignment to a string type should result in an appropriate coercion to the right value
  ASSERT_TRUE(acm->SetParsed("Namespace1.XYZ", "324"));
}

TEST_F(AutoConfigTest, VerifyDuplicateConfigAssignment) {
  AutoRequired<AutoConfigManager> acm;
  ASSERT_TRUE(acm->SetParsed("Namespace1.XYZ", "324"));
  ASSERT_TRUE(acm->SetParsed("Namespace2.XYZ", "1111"));

  AutoRequired<MyConfigurableClass> clz1;
  AutoRequired<MyConfigurableClass2> clz2;

  ASSERT_EQ(324, *clz1->m_myName);
  ASSERT_EQ(1111, clz2->m_myName);
}

TEST_F(AutoConfigTest, VerifySet) {
  AutoRequired<AutoConfigManager> acm;
  acm->Set("Namespace1.XYZ", 324);
  
  AutoRequired<MyConfigurableClass> clz1;
  ASSERT_EQ(324, clz1->m_myName);
  
  clz1->m_myName.Set(42);
  ASSERT_EQ(42, clz1->m_myName);
}

TEST_F(AutoConfigTest, ExtractKeyTestWin) {
  std::stringstream win("struct AutoConfigBase::ConfigTypeExtractor<struct Namespace1,struct XYZ>");
  
  ASSERT_STREQ(
    "Namespace1.XYZ",
    autowiring::ExtractKeyWin(win).c_str()
  ) << "Windows key extraction implementation mismatch";
}

class TypeWithoutAShiftOperator {
public:
  int foo;
};

TEST_F(AutoConfigTest, TypeWithoutAShiftOperatorTest) {
  AutoConfig<TypeWithoutAShiftOperator, struct MyValue> taso;

  AutoCurrentContext ctxt;
  AutoRequired<AutoConfigManager> mgr;

  // Indirect assignment should cause an exception
  ASSERT_ANY_THROW(mgr->Set("MyValue", "")) << "Expected a throw in a case where a configurable value was used which cannot be assigned";

  // Direct assignment should be supported still
  TypeWithoutAShiftOperator tasoVal;
  tasoVal.foo = 592;
  mgr->Set<TypeWithoutAShiftOperator>("MyValue", tasoVal);

  ASSERT_EQ(592, taso->foo) << "Value assignment did not result in an update to a non-serializable configuration field";
}