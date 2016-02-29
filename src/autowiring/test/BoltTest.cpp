// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/autowiring.h>
#include <autowiring/Bolt.h>
#include "TestFixtures/SimpleObject.hpp"
#include <string>
#include <iostream>

class BoltTest:
  public testing::Test
{};

struct Pipeline {};
struct OtherContext {};

class Listener:
  public Bolt<Pipeline>
{
public:
  bool hit = false;

  std::shared_ptr<CoreContext> createdContext;

  void ContextCreated(void) override {
    hit = true;

    createdContext = CoreContext::CurrentContext();
  }
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
  int count = 0;
};

class InjectsIntoEverything:
  public Bolt<>
{
public:
  void ContextCreated(void) override {
    AutoRequired<CountObject> derp;
    (derp->count)++;
  }

  int count = 0;
};

TEST_F(BoltTest, VerifySimpleInjection) {
  AutoCurrentContext ctxt;
  AutoEnable<InjectsIntoPipeline>();

  auto created = ctxt->Create<Pipeline>();

  // Verify that the SimpleObject didn't accidentally get injected out here:
  {
    Autowired<SimpleObject> so;
    ASSERT_FALSE(so.IsAutowired()) << "Object was injected into an outer scope by a bolt";
  }

  // Verify that the objecT DID get autowired where we expected it to be autowired
  CurrentContextPusher pshr(created);
  Autowired<SimpleObject> so;
  ASSERT_TRUE(so.IsAutowired()) << "Simple object was not injected as expected into a dependent context";
}

TEST_F(BoltTest, VerifyMapping) {
  // Trivial context creation check:
  AutoCreateContextT<Pipeline> ctxt1;

  // Now try to autowire a listener:
  AutoRequired<Listener> myListener;
  auto cleanup = MakeAtExit([myListener] {
    myListener->createdContext.reset();
  });

  // Create a second context, verify that the listener got the message:
  AutoCreateContextT<Pipeline> createdContext;

  // Verify we have a hit our bolt:
  ASSERT_TRUE(myListener->hit) << "The listener callback was not hit as expected";

  // Verify that the correct context was created:
  ASSERT_EQ(createdContext, myListener->createdContext) << "The context set to current for the listener callback was not the context that got created";
}

TEST_F(BoltTest, VerifyCreationBubbling) {
  // Leakage check:
  {
    Autowired<Listener> validation;
    ASSERT_FALSE(validation.IsAutowired());
  }

  // Put the listener in the parent context:
  AutoRequired<Listener> listener;
  auto cleanup = MakeAtExit([listener] {
    listener->createdContext.reset();
  });

  AutoCurrentContext outer;

  // Create a child context of the current one, and put the creator in there:
  AutoCreateContext childContext;
  CurrentContextPusher pshr(childContext);

  auto pipline = outer->Create<Pipeline>();

  // Check the listener to verify we had a hit:
  ASSERT_TRUE(listener->hit) << "The listener callback was not hit as expected";
}

TEST_F(BoltTest, VerifyMultipleInjection) {
  AutoCurrentContext ctxt;
  AutoEnable<InjectsIntoBoth>();

  auto created = ctxt->Create<Pipeline>();
  auto created2 = ctxt->Create<OtherContext>();

  // Verify that the SimpleObject didn't accidentally get injected out here:
  {
    Autowired<SimpleObject> so;
    ASSERT_FALSE(so.IsAutowired()) << "Object was injected into an outer scope by a bolt";
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
  AutoCurrentContext ctxt;
  AutoEnable<InjectsIntoEverything>();
  Autowired<CountObject> so;
  ASSERT_FALSE(so.IsAutowired()) << "CountObject injected into outer context";

  AutoCreateContext created;
  {
    CurrentContextPusher pshr(created);
    Autowired<CountObject> innerSo;
    ASSERT_TRUE(innerSo.IsAutowired()) << "CountObject not injected into anonymous context";
    ASSERT_EQ(1, innerSo->count) << "ContextCreated() called incorrect number of times";
  }

  auto created2 = ctxt->Create<Pipeline>();
  {
    CurrentContextPusher pshr(created2);
    Autowired<CountObject> innerSo;
    ASSERT_TRUE(innerSo.IsAutowired()) << "CountObject not injected into named context";
    ASSERT_EQ(1, innerSo->count) << "ContextCreated() called incorrect number of times";
  }
}

template<class target>
class TargetBoltable: public Boltable<target> {
  static int numCons;
  static int numDest;

public:
  static void InitializeNum() {
    numCons = 0;
    numDest = 0;
  }
  static int ConstructedNum() {return numCons;}
  static int DestructedNum() {return numDest;}

  TargetBoltable() {++numCons;}
  ~TargetBoltable() {++numDest;}
};

template<class target>
int TargetBoltable<target>::numCons = 0;
template<class target>
int TargetBoltable<target>::numDest = 0;

TEST_F(BoltTest, BoltBeforeContext) {
  AutoCurrentContext ctxt;
  TargetBoltable<Pipeline>::InitializeNum();
  AutoEnable<TargetBoltable<Pipeline>>();
  ASSERT_EQ(0, TargetBoltable<Pipeline>::ConstructedNum()) << "Instantiated Boltable without context";
  auto otherCtx = ctxt->Create<OtherContext>();
  ASSERT_EQ(0, TargetBoltable<Pipeline>::ConstructedNum()) << "Instantiated on creation of unmatched context";

  {
    auto created = ctxt->Create<Pipeline>();
    ASSERT_EQ(1, TargetBoltable<Pipeline>::ConstructedNum()) << "Failed to instantiate on creation of matching context";
  }
  ASSERT_EQ(1, TargetBoltable<Pipeline>::DestructedNum()) << "Failed to be destroyed with matching context";
}

TEST_F(BoltTest, BoltAfterContext) {
  AutoCurrentContext ctxt;
  TargetBoltable<OtherContext>::InitializeNum();
  TargetBoltable<Pipeline>::InitializeNum();

  {
    auto created = ctxt->Create<Pipeline>();
    AutoEnable<TargetBoltable<OtherContext>>();
    ASSERT_EQ(0, TargetBoltable<OtherContext>::ConstructedNum()) << "Instantiated Boltable without context";
    AutoEnable<TargetBoltable<Pipeline>>();
    ASSERT_EQ(1, TargetBoltable<Pipeline>::ConstructedNum()) << "Failed to instantiate on creation of matching context";
  }
  ASSERT_EQ(1, TargetBoltable<Pipeline>::DestructedNum()) << "Failed to be destroyed with matching context";
}
