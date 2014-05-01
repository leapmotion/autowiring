// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "PostConstructTest.h"
#include "Autowired.h"
#include "ContextMember.h"
#include "TestFixtures/SimpleObject.h"

using namespace std;

class ContextExposer:
  public CoreContext
{
public:
  size_t DeferredCount(void) const {return CoreContext::m_deferred.size();}
};

// Two classes to make up the cyclic dependency:
class A:
  public ContextMember
{
public:
  A(void):
    m_value(2)
  {}

  int m_value;

  int GetValue(void) const {return m_value;}
};

// For testing NotifyWhenAutowired with heirarchies
class Interface
{
public:
  virtual int GetValue() = 0;
};

class Implementation :
  public ContextMember,
  public Interface
{
public:
  virtual int GetValue() override { return 2; }
};

class Naive:
  public ContextMember
{
public:
  Naive(void) {
    if(!m_a)
      throw exception();
  }

  Autowired<A> m_a;
};

class Smarter:
  public ContextMember
{
public:
  Smarter(void):
    value(1)
  {
    m_a.NotifyWhenAutowired([this] () {
      this->value = m_a->GetValue();
    });
  }

  int value;
  Autowired<A> m_a;
};

class SmarterInterface
{
public:
  SmarterInterface(void):
    value(1)
  {
    m_interface.NotifyWhenAutowired([this] () {
      this->value = m_interface->GetValue();
    });
  }

  int value;
  Autowired<Interface> m_interface;
};

class FailedAutowiringInstance {
};

TEST_F(PostConstructTest, VerifyNaiveBehavior) {
  // Create a context and add just the naive class, to verify the problematic behavior:
  AutoCurrentContext ctxt;
  EXPECT_THROW(ctxt->Inject<Naive>(), std::exception) << "Naive class didn't throw an exception as expected";
}

TEST_F(PostConstructTest, VerifyExpectedDeferrmentCount) {
  AutoCurrentContext ctxt;

  // Add the smart class, which should introduce a single deferred count:
  ctxt->Inject<Smarter>();

  // Now test the count:
  EXPECT_EQ(
    1UL,
    ((ContextExposer&)*ctxt).DeferredCount()
  ) << "Unexpected number of deferred initializers";
}

TEST_F(PostConstructTest, VerifySmartBehavior) {
  AutoCurrentContext ctxt;

  // Add the smart class, which has a member that autowired type A
  ctxt->Inject<Smarter>();

  // Check that we can get the item we just injected
  Autowired<Smarter> smarter;
  EXPECT_EQ(1, smarter->value) << "Unexpected initial value of SmarterA instance";

  // Now inject A, and see if delayed autowiring has taken place:
  ctxt->Inject<A>();
  EXPECT_FALSE(!smarter->m_a.get()) << "Autowired member was not wired as expected";

  // Verify the value was updated by the notification routine
  EXPECT_EQ(2, smarter->value) << "Post-construction notification routine wasn't invoked as expected";
}

TEST_F(PostConstructTest, VerifySmartBehaviorWithInheritance) {
  AutoCurrentContext ctxt;

  // Add the smart classes, which should succeed
  ctxt->Inject<SmarterInterface>();

  //Initially value should be one, which is the default
  Autowired<SmarterInterface> smarterI;
  EXPECT_EQ(1, smarterI->value) << "Unexpected initial value of SmarterA instance";

  //Now add Implementation and check the wiring
  ctxt->Inject<Implementation>();
  EXPECT_FALSE(!smarterI->m_interface.get()) << "Autowired subclass was not wired as expected";

  EXPECT_EQ(2, smarterI->value) << "Post-construction notification routine wasn't invoked on subclass";
}

TEST_F(PostConstructTest, VerifyLoopingFailedAutowiring) {
  for(size_t i = 10; i--;) {
    Autowired<FailedAutowiringInstance> ignored;
    ASSERT_FALSE(ignored.IsAutowired()) << "Successfully autowired an instance that should not have been autowirable";
  }
}

TEST_F(PostConstructTest, VerifyTrivialNotifyWhenAutowired) {
  // Inject a type first:
  AutoRequired<SimpleObject>();

  // Now autowire, and add a registration:
  bool called = false;
  Autowired<SimpleObject> so;
  so.NotifyWhenAutowired([&called] { called = true; });

  ASSERT_TRUE(called) << "An autowiring notification was not invoked on an already-satisfied field as expected";
}