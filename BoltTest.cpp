// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "BoltTest.h"
#include "Autowired.h"
#include "Bolt.h"
#include "ContextCreator.h"
#include <string>
#include <iostream>

struct Pipeline {};

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

TEST_F(CreationListenerTest, VerifyMapping) {
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

TEST_F(CreationListenerTest, VerifyDescendantMapping) {
  // Leakage check:
  {
    Autowired<Listener> validation;
    ASSERT_FALSE(validation.IsAutowired());
  }

  // Put the creator in the parent context:
  AutoRequired<ContextCreator<Pipeline, std::wstring>> simpleCreator;
  std::shared_ptr<Listener> listener;

  // Create a child context of the current one, and put the listener in there:
  AutoCreateContext childContext;
  {
    CurrentContextPusher pshr(childContext);
    AutoRequired<Listener> myListener;
    listener = myListener;
  }

  // Dependent context broadcast check:
  simpleCreator->CreateContext(L"Simple");

  // Check the listener to verify we had a hit:
  EXPECT_TRUE(listener->hit) << "The listener callback was not hit as expected";
}
