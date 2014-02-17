// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "ScopeTest.h"
#include "GlobalCoreContext.h"
#include "TestFixtures/SimpleObject.h"
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


TEST_F(ScopeTest, VerifyAutowireSpecifiedContext){
  AutoCurrentContext ctxt;

  AutoCreateContext subCtxt;

  subCtxt->Add<A>();

  Autowired<A> aWired(subCtxt);
  EXPECT_TRUE(aWired) << "Autowired member not wired from the passed context";
}

TEST_F(ScopeTest, VerifyAutoRequireSpecifiedContext){
  AutoCreateContext subCtxt;
  AutoRequired<SimpleObject> aReq(subCtxt);
  Autowired<SimpleObject> aWired(subCtxt);
  Autowired<SimpleObject> aFail;

  EXPECT_TRUE(aWired) << "Autorequired member not added to the passed context";
  EXPECT_FALSE(aFail) << "Autorequired member added to the wrong context";
}

//This mangles the heap! why??  Using SimpleObject instead of A works fine!
//This beahvior was NOT introduced by the new context argument.
TEST_F(ScopeTest, AutowiringHeapMangle){
  AutoRequired<A> creator;
  Autowired<A> reference;
}