// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/SimpleInterface.hpp"
#include <iostream>

using namespace std;

class FactoryTest:
  public testing::Test
{};

/// <summary>
/// Compile-time check to ensure that unconstructable types are identified correctly
/// </summary>
class ClassWithIntegralCtor
{
public:
  ClassWithIntegralCtor(int) {}
};
static_assert(!autowiring::has_simple_constructor<ClassWithIntegralCtor>::value, "A class without a simple constructor was incorrectly identified as having one");

class ClassWithStaticNew:
  public CoreObject
{
public:
  ClassWithStaticNew(bool madeByFactory = false):
    m_madeByFactory(madeByFactory)
  {}

  const bool m_madeByFactory;

  // Factory method, for trivial factory construction:
  static ClassWithStaticNew* New(void) {
    return new ClassWithStaticNew(true);
  }
};

static_assert(autowiring::has_simple_constructor<ClassWithStaticNew>::value, "Class with default-argument constructor was not correctly detected as such ");
static_assert(autowiring::has_static_new<ClassWithStaticNew>::value, "Class with static allocator was not correctly detected as having one");
static_assert(!autowiring::has_static_new<CoreObject>::value, "Static New detected on a class that does not have a static New");

TEST_F(FactoryTest, VerifyFactoryCall) {
  // Try to create the static new type:
  AutoRequired<ClassWithStaticNew> si;

  // Verify the correct version was called:
  ASSERT_TRUE(si->m_madeByFactory) << "A factory method was not called on a type which provided a static factory New method";
}

class HasANontrivialFactoryNew:
  public ContextMember
{
public:
  HasANontrivialFactoryNew(int second, int first):
    m_first(first),
    m_second(second)
  {}

  static HasANontrivialFactoryNew* New(int first, int second) {
    return new HasANontrivialFactoryNew(second, first);
  }

  const int m_first;
  const int m_second;
};

static_assert(autowiring::has_static_new<HasANontrivialFactoryNew, int, int>::value, "Factory new not correctly identified on a type with a multi-argument factory new");

class HasAnIncorrectFactoryNew:
  public HasANontrivialFactoryNew
{};

static_assert(!autowiring::has_static_new<HasAnIncorrectFactoryNew>::value, "Factory new was incorrectly detected on a type where the return value of New was not implicitly castable to that type");

TEST_F(FactoryTest, CanForwardFactoryNew) {
  // Inject our type:
  auto hantfn = AutoCurrentContext()->Inject<HasANontrivialFactoryNew>(202, 203);

  // Validate that the factory new method was called as expected, and the ctor itself was not directly invoked:
  ASSERT_EQ(202, hantfn->m_first) << "Nontrivial factory-bearing type did not receive an argument as expected";
}

class HasFactoryNewAndRegularCtor:
  public ContextMember
{
public:
  HasFactoryNewAndRegularCtor(int first) :
    m_first(first),
    m_second(0)
  {}

  HasFactoryNewAndRegularCtor(int second, int first) :
    m_first(first),
    m_second(second)
  {}

  static HasFactoryNewAndRegularCtor* New(int first, int second) {
    return new HasFactoryNewAndRegularCtor(second, first);
  }

  const int m_first;
  const int m_second;
};

TEST_F(FactoryTest, CanUseBothStrategies) {
  // Inject type using the first strategy:
  auto first = AutoCreateContext()->Inject<HasFactoryNewAndRegularCtor>(101);
  ASSERT_EQ(101, first->m_first) << "Failed to construct a mixed-strategy type using a constructor strategy";

  // Now inject using the second strategy:
  auto second = AutoCreateContext()->Inject<HasFactoryNewAndRegularCtor>(201, 202);
  ASSERT_NE(202, second->m_first) << "Mixed-strategy construction strategy passed arguments directly to the constructor, rather than via New";
  ASSERT_EQ(201, second->m_first) << "Failed to construct a mixed-strategy type using a factory strategy";
}

class HasAliasedConstructorType:
  public ContextMember
{
private:
  HasAliasedConstructorType(long long value):
    m_value(value)
  {}

public:
  static HasAliasedConstructorType* New(long long value) {
    return new HasAliasedConstructorType(value);
  }

  long long m_value;
};

TEST_F(FactoryTest, CompatibleFactoryNewCall) {
  auto aliased = AutoCreateContext()->Inject<HasAliasedConstructorType>(22);
  ASSERT_EQ(22, aliased->m_value) << "Failed to pass a compatible value to an aliasing constructor type";
}

class FactoryNewAbstractBase:
  public ContextMember
{
public:
  static FactoryNewAbstractBase* New(void);
};

class FactoryNewConcreteType:
  public FactoryNewAbstractBase
{};

FactoryNewAbstractBase* FactoryNewAbstractBase::New(void) {
  return new FactoryNewConcreteType;
}

TEST_F(FactoryTest, VerifyCanAutowireActualType) {
  AutoRequired<FactoryNewAbstractBase> myBase;
  Autowired<FactoryNewConcreteType> concrete;

  ASSERT_TRUE(concrete.IsAutowired()) << "Failed to find the concrete derived type in a factory new construction in a case where it is known to exist";
}

class AcceptsString:
  public CoreObject
{
public:
  AcceptsString(const char* str) :
    str(str)
  {}

  static AcceptsString* New(const char* pstrNamespace) {
    return new AcceptsString(pstrNamespace);
  }

  const char* const str;
};

TEST_F(FactoryTest, StringLiteralCheck) {
  // This case can sometimes fail to compile because of the way we check for static new.
  // String literals are interpreted to be a reference to an array, and taking the address of such a type can
  // cause problems.
  AutoCurrentContext()->Inject<AcceptsString>("Abcd");
  Autowired<AcceptsString> as;

  ASSERT_TRUE(as.IsAutowired()) << "Standard injection with an argument did not correctly forward the argument";
  ASSERT_STREQ("Abcd", as->str) << "Constructed type was not constructed properly";
}
