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
  ASSERT_EQ(323, *mcc->m_myName) << "Configurable type did not receive a value as expected";
}

struct MyBoolClass {
  AutoConfig<bool, struct bool_space, struct my_bool> m_bool;
};

TEST_F(AutoConfigTest, VerifyBool) {
  AutoRequired<AutoConfigManager> acm;
  AutoRequired<MyBoolClass> clz1;
  
  acm->Set("bool_space.my_bool", true);
  ASSERT_TRUE(*clz1->m_bool);
  
  acm->SetParsed("bool_space.my_bool", "false");
  ASSERT_FALSE(*clz1->m_bool);
}

TEST_F(AutoConfigTest, VerifyPostHocAssignment) {
  // Inject the configurable type first
  AutoRequired<MyConfigurableClass> mcc;

  // Configuration manager must exist at this point as a consequence of the earlier construction
  Autowired<AutoConfigManager> acm;
  ASSERT_TRUE(acm.IsAutowired()) << "AutoConfig field did not inject a configuration manager into this context as expected";

  acm->Set("Namespace1.XYZ", 323);

  // Now inject the type which expects this value to be assigned:
  ASSERT_EQ(323, *mcc->m_myName) << "Configurable type did not receive a value as expected";
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
    ASSERT_EQ(1001, *mcc->m_myName) << "Configurable value obtained from a parent scope did not have the correct value";

    // This must work as expected--a local context override will rewrite configuration values in the local scope
    AutoRequired<AutoConfigManager> sub_mcc;
    sub_mcc->Set("Namespace1.XYZ", 1002);
    ASSERT_EQ(1002, *mcc->m_myName) << "Override of a configurable value in a derived class did not take place as expected";
  }
}

struct DefaultName {
  AutoConfig<int, struct defaultname1> m_def;
};

TEST_F(AutoConfigTest, DefaultNamespace) {
  AutoRequired<AutoConfigManager> acm;
  acm->Set("defaultname1", 123);
  
  AutoRequired<DefaultName> def;
  
  ASSERT_EQ(123, *def->m_def);
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
  ASSERT_EQ(1111, *clz2->m_myName);
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

struct NoShift {
  AutoConfig<TypeWithoutAShiftOperator, struct MyNoShift> m_noshift;
};

static_assert(has_stream<int>::value, "Stream operation not detected on a primitive type");
static_assert(!has_stream<TypeWithoutAShiftOperator>::value, "Stream operation detected on a type that should not have supported it");

TEST_F(AutoConfigTest, TypeWithoutAShiftOperatorTest) {
  AutoRequired<NoShift> noshift;

  AutoRequired<AutoConfigManager> mgr;

  // Indirect assignment should cause an exception
  ASSERT_ANY_THROW(mgr->Set("MyNoShift", "")) << "Expected a throw in a case where a configurable value was used which cannot be assigned";

  // Direct assignment should be supported still
  TypeWithoutAShiftOperator tasoVal;
  tasoVal.foo = 592;
  mgr->Set<TypeWithoutAShiftOperator>("MyNoShift", tasoVal);

  ASSERT_EQ(592, noshift->m_noshift->foo) << "Value assignment did not result in an update to a non-serializable configuration field";
}

TEST_F(AutoConfigTest, Callbacks) {
  AutoRequired<AutoConfigManager> acm;
  AutoRequired<MyConfigurableClass> mcc;
  
  acm->Set("Namespace1.XYZ", 4);
  
  mcc->m_myName += [](int val) {
    ASSERT_EQ(val, 42);
  };
  
  mcc->m_myName += [](int val) {
    ASSERT_EQ(val, 42);
  };
  
  acm->Set("Namespace1.XYZ", 42);
}

TEST_F(AutoConfigTest, NestedContexts) {
  // Set up contexts and members
  AutoCurrentContext ctxt_outer;
  std::shared_ptr<CoreContext> ctxt_middle = ctxt_outer->Create<void>();
  std::shared_ptr<CoreContext> ctxt_sibling = ctxt_outer->Create<void>();
  std::shared_ptr<CoreContext> ctxt_inner = ctxt_middle->Create<void>();
  
  AutoRequired<MyConfigurableClass> mcc_outer(ctxt_outer);
  AutoRequired<MyConfigurableClass> mcc_middle(ctxt_middle);
  AutoRequired<MyConfigurableClass> mcc_sibling(ctxt_sibling);
  AutoRequired<MyConfigurableClass> mcc_inner(ctxt_inner);
  
  AutoRequired<AutoConfigManager> acm_outer(ctxt_outer);
  AutoRequired<AutoConfigManager> acm_middle(ctxt_middle);
  
  // Set initial value
  acm_outer->Set("Namespace1.XYZ", 42);
  ASSERT_EQ(42, *mcc_outer->m_myName) << "Config value not set";
  ASSERT_EQ(42, *mcc_middle->m_myName) << "Config value not set in descendant context";
  ASSERT_EQ(42, *mcc_sibling->m_myName) << "Config value not set in descendant context";
  ASSERT_EQ(42, *mcc_inner->m_myName) << "Config value not set in descendant context";
  
  // Set middle, inner shouldn't be able to be set from outer after this
  bool callback_hit1 = false;
  mcc_inner->m_myName += [&callback_hit1](int) {
    callback_hit1 = true;
  };
  acm_middle->Set("Namespace1.XYZ", 1337);
  ASSERT_EQ(42, *mcc_outer->m_myName) << "Config value changed in outer context";
  ASSERT_EQ(42, *mcc_sibling->m_myName) << "Config value set from sibling context";
  ASSERT_EQ(1337, *mcc_middle->m_myName) << "Config value not set";
  ASSERT_EQ(1337, *mcc_inner->m_myName) << "Config value not set in child context";
  ASSERT_TRUE(callback_hit1) << "Callback not hit in inner context";
  
  // Set from outter, inner should be shielded by middle context
  mcc_inner->m_myName += [](int) {
    FAIL() << "This callback should never be hit";
  };
  
  // Make sure sibling context is not shielded
  bool callback_hit2 = false;
  mcc_sibling->m_myName += [&callback_hit2](int) {
    callback_hit2 = true;
  };
  
  // Set from outer, shouldn't effect middle or inner contexts
  acm_outer->Set("Namespace1.XYZ", 999);
  ASSERT_EQ(999, *mcc_outer->m_myName) << "Config value not set";
  ASSERT_EQ(1337, *mcc_middle->m_myName) << "Config value overwritten when value was set in this context";
  ASSERT_EQ(1337, *mcc_inner->m_myName) << "Config value overwritten when value was set in parent context";
  
  // Make sure sibling hit
  ASSERT_EQ(999, *mcc_sibling->m_myName) << "Value not set on sibling of context where value was previously set";
  ASSERT_TRUE(callback_hit2) << "Callback not called on sibling of context where value was previously set";
}

TEST_F(AutoConfigTest, Validators) {
  AutoRequired<AutoConfigManager> acm;
  AutoRequired<MyConfigurableClass> mcc;
  
  // Add validator to key that hasn't been set
  acm->AddValidator("Namespace1.XYZ", [](const AnySharedPointer& ptr){
    return true;
  });
  
  acm->Set("Namespace1.XYZ", 42);
  ASSERT_EQ(42, *mcc->m_myName);
  
  acm->AddValidator("Namespace1.XYZ", [](const AnySharedPointer& ptr){
    const int val = *ptr->as<int>();
    
    return (val < 50);
  });
  
  ASSERT_ANY_THROW(acm->Set("Namespace1.XYZ", 1337)) << "Should throw exception when setting invalid value";
  ASSERT_EQ(42, *mcc->m_myName);
  
  // Assert adding validator that doesn't validate current value throws execpetion
  ASSERT_ANY_THROW(acm->AddValidator("Namespace1.XYZ", [](const AnySharedPointer& ptr){
    const int val = *ptr->as<int>();
    
    return (val < 0);
  }));
}
