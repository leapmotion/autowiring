// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "FactoryTest.h"
#include "AutoFactory.h"
#include "TestFixtures/SimpleInterface.h"
#include <iostream>

using namespace std;

class ClassWithIntegralCtor:
  public SimpleInterface
{
public:
  ClassWithIntegralCtor(int) {}
};
static_assert(!has_simple_constructor<ClassWithIntegralCtor>::value, "A class without a simple constructor was incorrectly identified as having one");

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