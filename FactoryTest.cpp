// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "FactoryTest.h"
#include "AutoFactory.h"
#include <iostream>

using namespace std;

class SimpleInterface {
public:
  virtual void Method(void) = 0;
};

class ClassWithStaticAllocator:
  public SimpleInterface
{
public:
  ClassWithStaticAllocator(void):
    m_called(false)
  {}

  bool m_called;

  static ClassWithStaticAllocator* New(void) {return new ClassWithStaticAllocator();}

  void Method(void) override {
    m_called = true;
  }
};
static_assert(has_static_new<ClassWithStaticAllocator>::value, "Class with static allocator was not correctly detected as having one");

TEST_F(FactoryTest, VerifyFactoryWiring) {

}
