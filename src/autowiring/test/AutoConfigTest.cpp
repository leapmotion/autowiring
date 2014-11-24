// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/AutoConfig.h>
#include <autowiring/AutoConfigManager.h>

class AutoConfigTest:
  public testing::Test
{};

struct MyConfigurableClass {
  AutoConfig<int, struct XYZ> m_myName;
};

struct MyConfigurableClass2 {
  AutoConfig<int, struct XYZ> m_myName;
};

TEST_F(AutoConfigTest, VerifyCorrectDeconstruction) {
  AutoRequired<MyConfigurableClass> mcc;

  EXPECT_STREQ("MyConfigurableClass", mcc->m_myName.Class.c_str()) << "Configuration variable enclosing class was not correctly extracted";
  EXPECT_STREQ("XYZ", mcc->m_myName.Name.c_str()) << "Configuration variable name was not correctly extracted";
}

TEST_F(AutoConfigTest, VerifySimpleAssignment) {
  // Set an attribute in the manager before injecting anything:
  AutoRequired<AutoConfigManager> acm;
  acm->Set("MyConfigurableClass.XYZ", 323);

  // Now inject the type which expects this value to be assigned:
  AutoRequired<MyConfigurableClass> mcc;
  ASSERT_EQ(323, *mcc->m_myName) << "Configurable type did not receive a value as expected";
}

TEST_F(AutoConfigTest, VerifyPostHocAssignment) {
  // Inject the configurable type first
  AutoRequired<MyConfigurableClass> mcc;

  // Configuration manager must exist at this point as a consequence of the earlier construction
  Autowired<AutoConfigManager> acm;
  ASSERT_TRUE(acm.IsAutowired()) << "AutoConfig field did not inject a configuration manager into this context as expected";

  // 
  acm->Set("MyConfigurableClass.XYZ", 323);

  // Now inject the type which expects this value to be assigned:
  ASSERT_EQ(323, *mcc->m_myName) << "Configurable type did not receive a value as expected";
}

TEST_F(AutoConfigTest, VerifyRecursiveSearch) {
  AutoRequired<AutoConfigManager> acm;
  acm->Set("MyConfigurableClass.XYZ", 1001);

  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);

    // Now inject an element here, and verify that it was wired up as expected:
    AutoRequired<MyConfigurableClass> mcc;
    ASSERT_TRUE(mcc->m_myName.IsConfigured()) << "A configurable value was not configured as expected";
    ASSERT_EQ(1001, *mcc->m_myName) << "Configurable value obtained from a parent scope did not have the correct value";

    // This must work as expected--a local context override will rewrite configuration values in the local scope
    AutoRequired<AutoConfigManager> sub_mcc;
    sub_mcc->Set("MyConfigurableClass.XYZ", 1002);
    ASSERT_EQ(1002, *mcc->m_myName) << "Override of a configurable value in a derived class did not take place as expected";
  }
}

TEST_F(AutoConfigTest, VerifyParsedAssignment) {
  // We must also be able to support implicit string-to-type conversion via the shift operator for this type
  AutoRequired<AutoConfigManager> acm;

  // Direct assignment to a string should not work, the type isn't a string it's an int
  ASSERT_ANY_THROW(acm->Set("MyConfigurableClass.XYZ", "327")) << "An attempt to assign a value to an unrelated type did not generate an exception as expected";

  // Assignment to a string type should result in an appropriate coercion to the right value
  acm->SetParsed("MyConfigurableClass.XYZ", "324");
}

TEST_F(AutoConfigTest, VerifyDuplicateConfigAssignment) {
  AutoRequired<AutoConfigManager> acm;
  acm->SetParsed("MyConfigurableClass.XYZ", "324");
  acm->SetParsed("MyConfigurableClass2.XYZ", "1111");

  AutoRequired<MyConfigurableClass> clz1;
  AutoRequired<MyConfigurableClass2> clz2;

  ASSERT_EQ(324, *clz1->m_myName);
  ASSERT_EQ(1111, *clz2->m_myName);
}

namespace Outer {
  class Inner {
  public:
    AutoConfig<int, struct ZZZ> zzz;
  };
}

TEST_F(AutoConfigTest, NestedNamespaceTest) {
  AutoRequired<AutoConfigManager> acm;
  acm->SetParsed("Outer.Inner.ZZZ", "222");

  AutoRequired<Outer::Inner> inner;
  ASSERT_EQ(222, inner->zzz) << "Nested namespace type did not have the correct value";
}
