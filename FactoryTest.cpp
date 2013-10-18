// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "FactoryTest.h"
#include "AutoFactory.h"

class SimpleInterface {
public:
  virtual void Method(void) = 0;
};

class ConcreteInstance:
  public SimpleInterface
{
public:
  ConcreteInstance(void):
    m_called(false)
  {}

  bool m_called;

  void Method(void) override {
    m_called = true;
  }
};

class SimpleFactory:
  public AutoFactory<SimpleInterface>
{
public:
  SimpleFactory(void):
    m_called(false)
  {}

  virtual ~SimpleFactory() { }

  bool m_called;

  SimpleInterface* operator()(void) override {
    m_called = true;
    return new ConcreteInstance;
  }
};

TEST_F(FactoryTest, VerifyFactoryWiring) {
  // First insert our factory
  AutoRequired<SimpleFactory> factory;

  // Now we determine whether the factory gets invoked by trying to require an interface
  // TODO:  Change this to AutoRequired and correct the resulting compiler errors
  Autowired<SimpleInterface> instance;

  // Validation that the factory was called:
  EXPECT_TRUE(factory->m_called) << "The factory wasn't called when the interface instance was autowired";

  // Validation on the member:
  ASSERT_TRUE(instance.IsAutowired()) << "Factory didn't inject a SimpleInterface instance";

  // Validation on the type of the member
  ConcreteInstance* pInstance = dynamic_cast<ConcreteInstance*>(instance.get());
  ASSERT_TRUE(pInstance != nullptr);

  // Invoke the instance member and see what happens:
  instance->Method();
  EXPECT_TRUE(pInstance->m_called) << "Virtual call was not correctly honored by concrete instance";
}
