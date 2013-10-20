// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "FactoryTest.h"
#include "AutoFactory.h"
#include "TestFixtures/SimpleInterface.h"
#include <iostream>

using namespace std;

class ClassWithSimpleCtor:
  public SimpleInterface
{
public:
  ClassWithSimpleCtor(void):
    m_i(1)
  {}

  int m_i;

  void Method(void) override {
    m_called = true;
  }
};
static_assert(has_simple_constructor<ClassWithSimpleCtor>::value, "Class with a zero-argument constructor was not correctly identified as such");


class ClassWithIntegralCtor:
  public SimpleInterface
{
public:
  ClassWithIntegralCtor(int) {}
};
static_assert(!has_simple_constructor<ClassWithIntegralCtor>::value, "A class without a simple constructor was incorrectly identified as having one");


TEST_F(FactoryTest, VerifySimple) {
  std::weak_ptr<ClassWithSimpleCtor> weak;
  {
    std::shared_ptr<CoreContext> context = CoreContext::CurrentContext()->Create();
    CurrentContextPusher psher(context);

    // Trivial creation and destruction test:
    {
      AutoRequired<ClassWithSimpleCtor> req;
      ASSERT_TRUE(req.IsAutowired()) << "Class factory AutoRequired didn't create as expected";

      weak = req;
      EXPECT_EQ(1, req->m_i) << "Class factory didn't invoke the SimpleClass ctor";
    }
    EXPECT_FALSE(weak.expired()) << "CtorProxy object was prematurely free";
  }
  EXPECT_TRUE(weak.expired()) << "CtorProxy object leaked from its parent context";
}

TEST_F(FactoryTest, VerifyFactoryCall) {
  class ClassWithStaticNew
  {
  public:
    ClassWithStaticNew(bool madeByFactory = false):
      m_madeByFactory(madeByFactory)
    {}

    bool m_madeByFactory;

    // Factory method, for trivial factory construction:
    static ClassWithStaticNew* New(void) {
      return new ClassWithStaticNew(true);
    }
  };
  static_assert(has_static_new<ClassWithStaticNew>::value, "Class with static allocator was not correctly detected as having one");

  // Try to create the static new type:
  AutoRequired<ClassWithStaticNew> si;

  // Verify the correct version was called:
  ASSERT_TRUE(si->m_madeByFactory) << "A factory method was not called on a type which provided a static factory New method";
}

TEST_F(FactoryTest, VerifyCanRequireAbstract) {
  // Make a factory for SimpleInterface

  // Should invoke the factory constructor method:
  AutoRequired<SimpleInterface> si;
}