// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "FactoryTest.h"
#include "AutoFactory.h"
#include <iostream>

using namespace std;

class SimpleInterface {
public:
  SimpleInterface(void):
    m_called(false)
  {}

  bool m_called;

  virtual void Method(void) = 0;

  // Factory method, for trivial factory construction:
  static SimpleInterface* New(void);
};
static_assert(has_static_new<SimpleInterface>::value, "Class with static allocator was not correctly detected as having one");

/// <summary>
/// Simple constructor
/// </summary>
class ClassWithSimpleCtor:
  public SimpleInterface
{
public:
  void Method(void) override {
    m_called = true;
  }
};
static_assert(has_simple_constructor<ClassWithSimpleCtor>::value, "Class with a zero-argument constructor was not correctly identified as such");

class ClassWithIntegralCtor:
  public SimpleInterface
{
public:
  ClassWithIntegralCtor(int)
  {
  }
};
static_assert(!has_simple_constructor<ClassWithIntegralCtor>::value, "A class without a simple constructor was incorrectly identified as having one");

// Now we can define the factory method, once ClassWithSimpleCtor has been defined.
SimpleInterface* SimpleInterface::New(void) {
  return new ClassWithSimpleCtor();
}

TEST_F(FactoryTest, VerifyFactoryWiring) {

}
