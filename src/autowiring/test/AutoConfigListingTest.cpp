// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/AutoConfig.h>
#include <autowiring/AutoConfigListing.h>
#include <src/autowiring/AutoConfigParser.hpp>

class AutoConfigListingTest :
  public testing::Test
{};

struct Namespace1;
struct Namespace2;
struct XYZ;

TEST_F(AutoConfigListingTest, VerifySimpleAssignment) {
  // Set an attribute in the manager before injecting anything:
  AutoRequired<AutoConfigListing> acm;
  acm->Set("Namespace1.XYZ", 323);

  // Now inject the type which expects this value to be assigned:
  AutoConfig<int, Namespace1, XYZ> cfg;
  ASSERT_EQ(323, *cfg) << "Configurable type did not receive a value as expected";
}

struct NamespaceRoot;
struct NamespaceChild;

TEST_F(AutoConfigListingTest, VerifyNestedNamespace) {
  AutoRequired<AutoConfigListing> acm;
  acm->Set("NamespaceRoot.NamespaceChild.Namespace1.Namespace2.XYZ", 142);

  AutoConfig<int, struct NamespaceRoot, struct NamespaceChild, struct Namespace1, struct Namespace2, struct XYZ> cfg;
  ASSERT_EQ(142, *cfg);
}

struct MyBoolClass {
  AutoConfig<bool, struct bool_space, struct my_bool> m_bool;
};

TEST_F(AutoConfigListingTest, VerifyPostHocAssignment) {
  // Inject the configurable type first
  AutoConfig<int, Namespace1, XYZ> cfg;

  // Configuration manager must exist at this point as a consequence of the earlier construction
  Autowired<AutoConfigListing> acm;
  ASSERT_TRUE(acm.IsAutowired()) << "AutoConfig field did not inject a configuration manager into this context as expected";

  acm->Set("Namespace1.XYZ", 323);

  // Now inject the type which expects this value to be assigned:
  ASSERT_EQ(323, *cfg) << "Configurable type did not receive a value as expected";
}

TEST_F(AutoConfigListingTest, VerifyRecursiveSearch) {
  AutoRequired<AutoConfigListing> acm;
  acm->Set("Namespace1.XYZ", 1001);

  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);

    // Now inject an element here, and verify that it was wired up as expected:
    AutoConfig<int, Namespace1, XYZ> cfg;
    ASSERT_TRUE(cfg->IsConfigured()) << "A configurable value was not configured as expected";
    ASSERT_EQ(*cfg, 1001) << "Configurable value obtained from a parent scope did not have the correct value";

    // This must work as expected--a local context override will rewrite configuration values in the local scope
    AutoRequired<AutoConfigListing> sub_mcc;
    sub_mcc->Set("Namespace1.XYZ", 1002);
    ASSERT_EQ(1002, *cfg) << "Override of a configurable value in a derived class did not take place as expected";
  }
}

struct DefaultName {
  AutoConfig<int, struct defaultname1> m_def;
};

TEST_F(AutoConfigListingTest, DefaultNamespace) {
  AutoRequired<AutoConfigListing> acm;
  acm->Set("defaultname1", 123);
  
  AutoRequired<DefaultName> def;
  
  ASSERT_EQ(*def->m_def, 123);
}

TEST_F(AutoConfigListingTest, VerifyParsedAssignment) {
  // We must also be able to support implicit string-to-type conversion via the shift operator for this type
  AutoRequired<AutoConfigListing> acm;

  // Direct assignment to a string should not work, the type isn't a string it's an int
  ASSERT_ANY_THROW(acm->Set("Namespace1.XYZ", "327")) << "An attempt to assign a value to an unrelated type did not generate an exception as expected";
  
  ASSERT_ANY_THROW(acm->Set("Namespace1.XYZ", 3.0)) << "An attempt to assign a value to an unrelated type did not generate an exception as expected";

  // Assignment to a string type should result in an appropriate coercion to the right value
  ASSERT_TRUE(acm->SetParsed("Namespace1.XYZ", "324"));
}

TEST_F(AutoConfigListingTest, VerifyDuplicateConfigAssignment) {
  AutoRequired<AutoConfigListing> acm;
  ASSERT_TRUE(acm->SetParsed("Namespace1.XYZ", "324"));
  ASSERT_TRUE(acm->SetParsed("Namespace2.XYZ", "1111"));

  AutoConfig<int, Namespace1, XYZ> clz1;
  AutoConfig<int, Namespace2, XYZ> clz2;

  ASSERT_EQ(*clz1, 324);
  ASSERT_EQ(*clz2, 1111);
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

TEST_F(AutoConfigListingTest, TypeWithoutAShiftOperatorTest) {
  AutoRequired<NoShift> noshift;

  AutoRequired<AutoConfigListing> mgr;

  // Indirect assignment should cause an exception
  ASSERT_ANY_THROW(mgr->Set("MyNoShift", "")) << "Expected a throw in a case where a configurable value was used which cannot be assigned";

  // Direct assignment should be supported still
  TypeWithoutAShiftOperator tasoVal;
  tasoVal.foo = 592;
  mgr->Set<TypeWithoutAShiftOperator>("MyNoShift", tasoVal);

  ASSERT_EQ((*noshift->m_noshift)->foo, 592) << "Value assignment did not result in an update to a non-serializable configuration field";
}

TEST_F(AutoConfigListingTest, Callbacks) {
  AutoRequired<AutoConfigListing> acm;
  AutoConfig<int, Namespace1, XYZ> cfg;
  
  acm->Set("Namespace1.XYZ", 4);
  
  *cfg += [](int val) {
    ASSERT_EQ(val, 42);
  };
  
  *cfg += [](int val) {
    ASSERT_EQ(val, 42);
  };
  
  acm->Set("Namespace1.XYZ", 42);
}

struct MyConfigurableClass {
  AutoConfig<int, Namespace1, XYZ> cfg;
};
struct MyConfigurableClass2 {
  AutoConfig<int, Namespace2, XYZ> cfg;
};

TEST_F(AutoConfigListingTest, NestedContexts) {
  // Set up contexts and members
  AutoCurrentContext ctxt_outer;
  std::shared_ptr<CoreContext> ctxt_middle = ctxt_outer->Create<void>();
  std::shared_ptr<CoreContext> ctxt_sibling = ctxt_outer->Create<void>();
  std::shared_ptr<CoreContext> ctxt_inner = ctxt_middle->Create<void>();
  std::shared_ptr<CoreContext> ctxt_no_manager = ctxt_inner->Create<void>();
  std::shared_ptr<CoreContext> ctxt_leaf = ctxt_no_manager->Create<void>();
  
  AutoRequired<MyConfigurableClass> mcc_outer(ctxt_outer);
  AutoRequired<MyConfigurableClass> mcc_middle(ctxt_middle);
  AutoRequired<MyConfigurableClass> mcc_sibling(ctxt_sibling);
  AutoRequired<MyConfigurableClass> mcc_inner(ctxt_inner);
  AutoRequired<MyConfigurableClass> mcc_leaf(ctxt_leaf);
  
  AutoRequired<AutoConfigListing> acm_outer(ctxt_outer);
  AutoRequired<AutoConfigListing> acm_middle(ctxt_middle);
  AutoRequired<AutoConfigListing> acm_leaf(ctxt_leaf);
  
  // Set initial value
  acm_outer->Set("Namespace1.XYZ", 42);
  ASSERT_EQ(42, *mcc_outer->cfg) << "Config value not set";
  ASSERT_EQ(42, *mcc_middle->cfg) << "Config value not set in descendant context";
  ASSERT_EQ(42, *mcc_sibling->cfg) << "Config value not set in descendant context";
  ASSERT_EQ(42, *mcc_inner->cfg) << "Config value not set in descendant context";
  ASSERT_EQ(42, *mcc_leaf->cfg) << "Config value not set in desendant context";
  ASSERT_TRUE(acm_middle->IsInherited("Namespace1.XYZ")) << "Inherited key not marked as such";
  ASSERT_TRUE(acm_leaf->IsInherited("Namespace1.XYZ")) << "Inherited key not marked as such";
  
  // Set middle, inner shouldn't be able to be set from outer after this
  bool callback_hit1 = false;
  *mcc_inner->cfg += [&callback_hit1](int) {
    callback_hit1 = true;
  };
  acm_middle->Set("Namespace1.XYZ", 1337);
  ASSERT_EQ(42, *mcc_outer->cfg) << "Config value changed in outer context";
  ASSERT_EQ(42, *mcc_sibling->cfg) << "Config value set from sibling context";
  ASSERT_EQ(1337, *mcc_middle->cfg) << "Config value not set";
  ASSERT_EQ(1337, *mcc_inner->cfg) << "Config value not set in child context";
  ASSERT_EQ(1337, *mcc_leaf->cfg) << "Config value not set in leaf context";
  ASSERT_TRUE(callback_hit1) << "Callback not hit in inner context";
  
  // Set from outter, inner should be shielded by middle context
  *mcc_inner->cfg += [](int) {
    FAIL() << "This callback should never be hit";
  };
  
  // Make sure sibling context is not shielded
  bool callback_hit2 = false;
  *mcc_sibling->cfg += [&callback_hit2](int) {
    callback_hit2 = true;
  };
  
  // Set from outer, shouldn't effect middle or inner contexts
  acm_outer->Set("Namespace1.XYZ", 999);
  ASSERT_EQ(999, *mcc_outer->cfg) << "Config value not set";
  ASSERT_EQ(1337, *mcc_middle->cfg) << "Config value overwritten when value was set in this context";
  ASSERT_EQ(1337, *mcc_inner->cfg) << "Config value overwritten when value was set in parent context";
  
  // Make sure sibling hit
  ASSERT_EQ(999, *mcc_sibling->cfg) << "Value not set on sibling of context where value was previously set";
  ASSERT_TRUE(callback_hit2) << "Callback not called on sibling of context where value was previously set";
}

struct ValidatedKey{
  static bool Validate(const int& value) {
    return value > 5;
  }
};

struct MyValidatedClass{
  AutoConfig<int, ValidatedKey> m_config;
};

TEST_F(AutoConfigListingTest, Validators) {
  AutoRequired<AutoConfigListing> acm;
  
  ASSERT_ANY_THROW(acm->Set("ValidatedKey", 2)) << "AutoConfigListing didn't regect invalid value";
  
  AutoRequired<MyValidatedClass> valid;
  
  acm->Set("ValidatedKey", 42);
  ASSERT_EQ(42, *valid->m_config) << "Value not set for key";
  
  ASSERT_ANY_THROW(acm->Set("ValidatedKey", 1)) << "AutoConfigListing didn't regect invalid value";
  ASSERT_EQ(42, *valid->m_config) << "Value not set for key";
  
  acm->Set("ValidatedKey", 1337);
  ASSERT_EQ(1337, *valid->m_config) << "Value not set for key";
}

struct OuterCtxt{};
struct MiddleCtxt{};
struct InnerCtxt{};
TEST_F(AutoConfigListingTest, ListingConfigs) {
  AutoCreateContextT<OuterCtxt> ctxt_outer;
  auto ctxt_middle = ctxt_outer->Create<MiddleCtxt>();
  auto ctxt_inner = ctxt_middle->Create<InnerCtxt>();

  AutoRequired<AutoConfigListing> acm_outer(ctxt_outer);
  AutoRequired<AutoConfigListing> acm_inner(ctxt_inner);

  int callback_outer = 0;
  acm_outer->AddCallback([&callback_outer](const AutoConfigVarBase& ptr) {
    ++callback_outer;
  });

  int callback_inner = 0;
  acm_inner->AddCallback([&callback_inner](const AutoConfigVarBase& ptr) {
    ++callback_inner;
  });

  AutoRequired<MyConfigurableClass> var1_inner(ctxt_inner);
  *var1_inner->cfg = 1;
 
  ASSERT_EQ(0, callback_outer) << "OnAdded fired incorrectly";
  ASSERT_EQ(1, callback_inner) << "Callback fired incorrectly";
  ASSERT_EQ(0, acm_outer->GetLocalKeys().size()) << "Incorrect number of keys found in outer context";
  ASSERT_EQ(1, acm_inner->GetLocalKeys().size()) << "Incorrect number of keys found in inner context";

  ASSERT_EQ(1, callback_inner) << "Callback not called on existing keys";

  AutoRequired<MyConfigurableClass> var1_outer(ctxt_outer);
  *var1_outer->cfg = 2;

  ASSERT_EQ(1, acm_outer->GetLocalKeys().size()) << "Incorrect number of keys found in outer context";
  ASSERT_EQ(1, acm_inner->GetLocalKeys().size()) << "Incorrect number of keys found in inner context";

  AutoRequired<MyConfigurableClass2> var2_outer(ctxt_outer);
  AutoRequired<MyConfigurableClass2> var2_inner(ctxt_inner);

  ASSERT_EQ(1, acm_outer->GetLocalKeys().size()) << "Constructing an uninitialized config incremented the local count";

  *var2_outer->cfg = 3;

  ASSERT_EQ(2, acm_outer->GetLocalKeys().size()) << "Incorrect number of local keys found in outer context";
  ASSERT_EQ(1, acm_inner->GetLocalKeys().size()) << "Incorrect number of local keys found in inner context";

  ASSERT_EQ(2, callback_outer) << "Outer callback called an incorrect number of times";
  ASSERT_EQ(1, callback_inner) << "Inner callback called an incorrect number of times";

  ASSERT_EQ(*var2_inner->cfg, *var2_outer->cfg) << "Value did not get set in child context";

  auto keys_outer = acm_outer->GetLocalKeys();
  ASSERT_EQ(var1_outer->cfg->m_key, keys_outer[0]) << "Keys listed out of construction order";
  ASSERT_EQ(var2_outer->cfg->m_key, keys_outer[1]) << "Keys listed out of construction order";

  auto keys_inner = acm_inner->GetLocalKeys();
  ASSERT_EQ(var1_inner->cfg->m_key, keys_inner[0]) << "Keys listed out of construction order";
}
