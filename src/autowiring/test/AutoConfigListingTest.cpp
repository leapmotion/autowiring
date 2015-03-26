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
/*
TEST_F(AutoConfigListingTest, VerifyRecursiveSearch) {
  AutoRequired<AutoConfigListing> acm;
  acm->Set("Namespace1.XYZ", 1001);

  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);

    // Now inject an element here, and verify that it was wired up as expected:
    AutoRequired<MyConfigurableClass> mcc;
    ASSERT_TRUE(mcc->m_myName->IsConfigured()) << "A configurable value was not configured as expected";
    ASSERT_EQ(1001, *mcc->m_myName) << "Configurable value obtained from a parent scope did not have the correct value";

    // This must work as expected--a local context override will rewrite configuration values in the local scope
    AutoRequired<AutoConfigListing> sub_mcc;
    sub_mcc->Set("Namespace1.XYZ", 1002);
    ASSERT_EQ(1002, *mcc->m_myName) << "Override of a configurable value in a derived class did not take place as expected";
  }
}

struct DefaultName {
  AutoConfig<int, struct defaultname1> m_def;
};

TEST_F(AutoConfigListingTest, DefaultNamespace) {
  AutoRequired<AutoConfigListing> acm;
  acm->Set("defaultname1", 123);
  
  AutoRequired<DefaultName> def;
  
  ASSERT_EQ(123, *def->m_def);
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

  AutoRequired<MyConfigurableClass> clz1;
  AutoRequired<MyConfigurableClass2> clz2;

  ASSERT_EQ(324, *clz1->m_myName);
  ASSERT_EQ(1111, *clz2->m_myName);
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

  ASSERT_EQ(592, noshift->m_noshift->foo) << "Value assignment did not result in an update to a non-serializable configuration field";
}

TEST_F(AutoConfigListingTest, Callbacks) {
  AutoRequired<AutoConfigListing> acm;
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
  ASSERT_EQ(42, *mcc_outer->m_myName) << "Config value not set";
  ASSERT_EQ(42, *mcc_middle->m_myName) << "Config value not set in descendant context";
  ASSERT_EQ(42, *mcc_sibling->m_myName) << "Config value not set in descendant context";
  ASSERT_EQ(42, *mcc_inner->m_myName) << "Config value not set in descendant context";
  ASSERT_EQ(42, *mcc_leaf->m_myName) << "Config value not set in desendant context";
  EXPECT_TRUE(acm_middle->IsInherited("Namespace1.XYZ")) << "Inherited key not marked as such";
  EXPECT_TRUE(acm_leaf->IsInherited("Namespace1.XYZ")) << "Inherited key not marked as such";
  
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
  ASSERT_EQ(1337, *mcc_leaf->m_myName) << "Config value not set in leaf context";
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

TEST_F(AutoConfigListingTest, DirectAssignemnt) {
  AutoConfig<int, struct Namespace1, struct XYZ> var;
  var = 10;
  ASSERT_EQ(10, *var);

  AutoRequired<MyConfigurableClass> containsVar;

  ASSERT_EQ(10, *var);
  ASSERT_EQ(10, *containsVar->m_myName);
}


struct ComplexValue {
  int a;
  int b;
  int c;

  ComplexValue(int nA, int nB, int nC) : a(nA), b(nB), c(nC) {}
  ComplexValue(int repeated) : a(repeated), b(repeated), c(repeated) {}
};

struct MyComplexValueClass {
  AutoConfig<ComplexValue, struct Namespace1, struct MyCxValue> m_cfg;
  AutoConfig<ComplexValue, struct Namespace1, struct MyCxValue2> m_cfg2 = ComplexValue{ 10, 15, 30 };

  MyComplexValueClass() : m_cfg(ComplexValue{ 2, 20, 20 }) {}
};

TEST_F(AutoConfigListingTest, ComplexConstruction){
  AutoRequired<AutoConfigListing> mgr;
  ASSERT_FALSE(mgr->IsConfigured("Namespace1.MyCxValue"));
  
  AutoConfig<ComplexValue, Namespace1, MyCxValue> defaultConstructed;

  ASSERT_FALSE(mgr->IsConfigured("Namespace1.MyCxValue")) << "Improperly set config value when default constructing AutoConfig";

  AutoConfig<ComplexValue, Namespace1, MyCxValue> fancyConstructed(1, 2, 3);
  
  ASSERT_TRUE(mgr->IsConfigured("Namespace1.MyCxValue")) << "Initializing constructor did not set config value";
  ASSERT_EQ(fancyConstructed->a, 1) << "Initializing constructor did not set config value";
  ASSERT_EQ(fancyConstructed->b, 2) << "Initializing constructor did not set config value";
  ASSERT_EQ(fancyConstructed->c, 3) << "Initializing constructor did not set config value";


  AutoConfig<ComplexValue, Namespace1, MyCxValue> fancy2(7);
  ASSERT_EQ(fancy2->a, 1) << "Second Initalizing constructor overrode the first!";
  ASSERT_EQ(fancy2->b, 2) << "Second Initalizing constructor overrode the first!";
  ASSERT_EQ(fancy2->c, 3) << "Second Initalizing constructor overrode the first!";
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

  AutoRequired<MyConfigurableClass> var1_inner(ctxt_inner);
  var1_inner->m_myName = 1;
 
  ASSERT_EQ(0, acm_outer->GetLocalKeys().size()) << "Incorrect number of keys found in outer context";
  ASSERT_EQ(1, acm_inner->GetLocalKeys().size()) << "Incorrect number of keys found in inner context";

  int callback_outer = 0;
  acm_outer->AddCallback([&callback_outer](const std::string& key, const AnySharedPointer& ptr) {
    ++callback_outer;
  });

  int callback_inner = 0;
  acm_inner->AddCallback([&callback_inner](const std::string& key, const AnySharedPointer& ptr) {
    ++callback_inner;
  });

  ASSERT_EQ(1, callback_inner) << "Callback not called on existing keys";

  AutoRequired<MyConfigurableClass2> var1_outer(ctxt_outer);
  var1_outer->m_myName = 2;

  ASSERT_EQ(1, acm_outer->GetLocalKeys().size()) << "Incorrect number of keys found in outer context";
  ASSERT_EQ(1, acm_inner->GetLocalKeys().size()) << "Incorrect number of keys found in inner context";

  AutoRequired<MyConfigurableClass> var2_outer(ctxt_outer);
  var2_outer->m_myName = 3;

  ASSERT_EQ(2, acm_outer->GetLocalKeys().size()) << "Incorrect number of keys found in outer context";
  ASSERT_EQ(1, acm_inner->GetLocalKeys().size()) << "Incorrect number of keys found in inner context";

  ASSERT_EQ(2, callback_outer) << "Outer callback called an incorrect number of times";
  ASSERT_EQ(1, callback_inner) << "Inner callback called an incorrect number of times";

  auto keys_outer = acm_outer->GetLocalKeys();
  ASSERT_EQ(var1_outer->m_myName.m_key, keys_outer[0]) << "Keys listed out of construction order";
  ASSERT_EQ(var2_outer->m_myName.m_key, keys_outer[1]) << "Keys listed out of construction order";

  auto keys_inner = acm_inner->GetLocalKeys();
  ASSERT_EQ(var1_inner->m_myName.m_key, keys_inner[0]) << "Keys listed out of construction order";
}
*/