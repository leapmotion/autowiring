#include "stdafx.h"
#include <gtest/gtest.h>

class ForeignType {
public:
  typedef void base;
  virtual ~ForeignType(void) = 0;

  virtual void foo(void) = 0;
};

class ForeignTypeImpl:
  public ForeignType
{
  typedef ForeignType base;

  virtual void foo(void) {}
};

class ForeignFactoryA {
public:
  ForeignType* New(void) {
    return new ForeignTypeImpl;
  }
};

class ForeignFactoryB {
public:
  std::tuple<ForeignType*, ForeignTypeImpl*> New(void) {
    ForeignTypeImpl* rv;
    return std::tuple<ForeignType*, ForeignTypeImpl*>(rv, rv);
  }
};

class ForeignFactoryC {
public:
  void New(ForeignType*& rv) {
    rv = new ForeignTypeImpl;
  }
};

// These include statements placed here intentionally to ensure that the above test fixture classes
// do not come to rely on anything actually defined in autowiring.
#include <autowiring/autowiring.h>
#include <autowiring/member_new_type.h>

using namespace autowiring;

static_assert(member_new_type<ForeignFactoryA>::factory == factorytype::single_ret, "Single return member return type not correctly extracted");
static_assert(member_new_type<ForeignFactoryB>::factory == factorytype::multi_ret, "Multi-ret member return type not correctly extracted");
static_assert(member_new_type<ForeignFactoryC>::factory == factorytype::multi_byref, "Multi-ret byref return member return type not correctly extracted");

class ForeignTest:
  public testing::Test
{};



TEST_F(ForeignTest, ForeignFactoryTest) {
  // Inject the factory, this should cause it to be registered as a factory
  AutoRequired<ForeignFactoryA> factory;

  // Now try to use the factory to construct our unrelated type.
  // Formerly, attempting to AutoRequire an interface would fail; this succeeds because we
  // now permit a factory to satisfy the request at runtime rather than requiring definite
  // knowledge on how to perform the satisfaction at compile time.
  AutoRequired<ForeignType> ft;
  ASSERT_TRUE(ft.IsAutowired()) << "Factory-constructed type was not autowired as expected";
}