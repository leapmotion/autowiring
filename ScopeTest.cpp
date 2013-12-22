// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "ScopeTest.h"
#include "GlobalCoreContext.h"
#include "Autowired.h"

TEST_F(ScopeTest, VerifyGlobalExists) {
  // Verify that we at least get a global scope
  AutoGlobalContext global;
  EXPECT_TRUE(nullptr != global.get()) << "Failed to autowire the global context";
}

class A : public ContextMember {};
class B : public ContextMember {};

TEST_F(ScopeTest, VerifyInherit) {
  AutoCurrentContext ctxt;
  
  //Add a member to the current context
  ctxt->Add<A>();

  std::shared_ptr<CoreContext> pContext = ctxt->CreateAnonymous();
  //Create and switch to a sub-context
  {
    CurrentContextPusher pusher;
    pContext->SetCurrent();

    EXPECT_TRUE(ctxt.get() != pContext.get()) << "Failed to create a sub-context";

    //try and autowire a member from the parent context
    Autowired<A> autoA;
    EXPECT_FALSE(!autoA.get()) << "Autowired member not wired from parent context";

    //add a member in the subcontext
    pContext->Add<B>();
  }

  Autowired<B> autoB;
  EXPECT_TRUE(!autoB.get()) << "Autowired member wired from sub-context";
}