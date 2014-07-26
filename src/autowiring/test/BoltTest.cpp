// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "BoltTest.hpp"
#include <autowiring/Autowired.h>
#include <autowiring/Bolt.h>
#include <autowiring/ContextCreator.h>
#include "TestFixtures/SimpleObject.hpp"
#include <string>
#include <iostream>

struct Pipeline {};
struct OtherContext {};

class Listener:
  public Bolt<Pipeline>
{
public:
  Listener(void):
    hit(false)
  {}

  bool hit;

  std::shared_ptr<CoreContext> createdContext;

  void ContextCreated(void) override {
    hit = true;

    createdContext = CoreContext::CurrentContext();
  }
};

class Creator:
  public ContextCreator<Pipeline, std::string>
{
};

class InjectsIntoPipeline:
  public Bolt<Pipeline>
{
public:
  void ContextCreated(void) override {
    AutoRequired<SimpleObject>();
  }
};

class InjectsIntoBoth:
  public Bolt<Pipeline,OtherContext>
{
public:
  void ContextCreated(void) override {
    AutoRequired<SimpleObject>();
  }
};

struct CountObject:
  ContextMember
{
  CountObject():
    count(0)
  {}

  int count;
};

class InjectsIntoEverything:
  public Bolt<>
{
public:
  InjectsIntoEverything():
    count(0)
  {}

  void ContextCreated(void) override {
    AutoRequired<CountObject> derp;
    (derp->count)++;
  }

  int count;
};

TEST_F(BoltTest, VerifySimpleInjection) {
  AutoEnable<InjectsIntoPipeline>();

  auto created = m_create->Create<Pipeline>();

  // Verify that the SimpleObject didn't accidentally get injected out here:
  {
    Autowired<SimpleObject> so;
    EXPECT_FALSE(so.IsAutowired()) << "Object was injected into an outer scope by a bolt";
  }

  // Verify that the objecT DID get autowired where we expected it to be autowired
  CurrentContextPusher pshr(created);
  Autowired<SimpleObject> so;
  ASSERT_TRUE(so.IsAutowired()) << "Simple object was not injected as expected into a dependent context";
}

TEST_F(BoltTest, VerifyMapping) {
  ContextCreator<Pipeline, std::wstring> simpleCreator;

  // Trivial context creation check:
  {
    auto ctxt = simpleCreator.CreateContext(L"Simple");
    ASSERT_TRUE(ctxt.second && ctxt.first.get()) << "Initial context creation did not succeed as expected";
  }

  // Now try to autowire a listener:
  AutoRequired<Listener> myListener;

  // Create a second context, verify that the listener got the message:
  std::shared_ptr<CoreContext> createdContext = simpleCreator.CreateContext(L"Simple2").first;

  // Verify we have a hit our bolt:
  EXPECT_TRUE(myListener->hit) << "The listener callback was not hit as expected";

  // Verify that the correct context was created:
  EXPECT_EQ(createdContext, myListener->createdContext) << "The context set to current for the listener callback was not the context that got created";
}

TEST_F(BoltTest, VerifyCreationBubbling) {
  // Leakage check:
  {
    Autowired<Listener> validation;
    ASSERT_FALSE(validation.IsAutowired());
  }

  // Put the listener in the parent context:
  AutoRequired<Listener> listener;

  // Create a child context of the current one, and put the creator in there:
  AutoCreateContext childContext;
  std::shared_ptr<ContextCreator<Pipeline, std::wstring>> simpleCreator = (
    CurrentContextPusher(childContext),
    AutoRequired<ContextCreator<Pipeline, std::wstring>>()
  );

  // Dependent context broadcast check:
  simpleCreator->CreateContext(L"Simple");

  // Check the listener to verify we had a hit:
  EXPECT_TRUE(listener->hit) << "The listener callback was not hit as expected";
}

TEST_F(BoltTest, VerifyMultipleInjection) {
  AutoEnable<InjectsIntoBoth>();

  auto created = m_create->Create<Pipeline>();
  auto created2 = m_create->Create<OtherContext>();

  // Verify that the SimpleObject didn't accidentally get injected out here:
  {
    Autowired<SimpleObject> so;
    EXPECT_FALSE(so.IsAutowired()) << "Object was injected into an outer scope by a bolt";
  }

  // Verify that the objecT DID get autowired where we expected it to be autowired
  {
    CurrentContextPusher pshr(created);
    Autowired<SimpleObject> so;
    ASSERT_TRUE(so.IsAutowired()) << "Simple object was not injected as expected into a dependent context";
  }
  {
    CurrentContextPusher pshr(created2);
    Autowired<SimpleObject> so;
    ASSERT_TRUE(so.IsAutowired()) << "Simple object was not injected as expected into a dependent context";
  }
}

TEST_F(BoltTest, EmptyBolt) {
  AutoEnable<InjectsIntoEverything>();
  Autowired<CountObject> so;
  EXPECT_FALSE(so.IsAutowired()) << "CountObject injected into outer context";

  AutoCreateContext created;
  {
    CurrentContextPusher pshr(created);
    Autowired<CountObject> innerSo;
    ASSERT_TRUE(innerSo.IsAutowired()) << "CountObject not injected into anonymous context";
    ASSERT_EQ(1, innerSo->count) << "ContextCreated() called incorrect number of times";
  }

  auto created2 = m_create->Create<Pipeline>();
  {
    CurrentContextPusher pshr(created2);
    Autowired<CountObject> innerSo;
    ASSERT_TRUE(innerSo.IsAutowired()) << "CountObject not injected into named context";
    ASSERT_EQ(1, innerSo->count) << "ContextCreated() called incorrect number of times";
  }
}
