// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ScopeTest.hpp"
#include "TestFixtures/SimpleObject.hpp"
#include <autowiring/Autowired.h>
#include <autowiring/GlobalCoreContext.h>

TEST_F(ScopeTest, VerifyGlobalExists) {
  // Verify that we at least get a global scope
  AutoGlobalContext global;
  EXPECT_TRUE(nullptr != global.get()) << "Failed to autowire the global context";
}

class A : public ContextMember {};
class B : public ContextMember {};
class C : public ContextMember {};
class D : public ContextMember {};

TEST_F(ScopeTest, VerifyInherit) {
  AutoCurrentContext ctxt;

  //Add a member to the current context
  ctxt->Inject<A>();

  AutoCreateContext pContext(ctxt);
  //Create and switch to a sub-context
  {
    CurrentContextPusher pusher;
    pContext->SetCurrent();

    EXPECT_TRUE(ctxt.get() != pContext.get()) << "Failed to create a sub-context";

    //try and autowire a member from the parent context
    Autowired<A> autoA;
    EXPECT_FALSE(!autoA.get()) << "Autowired member not wired from parent context";

    //add a member in the subcontext
    pContext->Inject<B>();
  }

  Autowired<B> autoB;
  EXPECT_TRUE(!autoB.get()) << "Autowired member wired from sub-context";
}

struct NoSimpleConstructor:
  public ContextMember
{
  NoSimpleConstructor(int val):
    value(val)
  {}

  const int value;
};

TEST_F(ScopeTest, AddWithArguments){
  //Add context member with non-simple constructor
  AutoCurrentContext ctxt;

  ctxt->Construct<NoSimpleConstructor>(10);

  Autowired<NoSimpleConstructor> wired;

  ASSERT_TRUE(wired.IsAutowired());
  ASSERT_EQ(10, wired->value);
}

TEST_F(ScopeTest, StaticInject){
  Autowired<A> preA;
  Autowired<B> preB;

  EXPECT_FALSE(preA.IsAutowired());
  EXPECT_FALSE(preB.IsAutowired());

  CoreContext::InjectCurrent<A,B>();

  Autowired<A> a;
  Autowired<B> b;

  EXPECT_TRUE(a.IsAutowired());
  EXPECT_TRUE(b.IsAutowired());
}

TEST_F(ScopeTest, VerifyAutowireSpecifiedContext){
  AutoCurrentContext ctxt;

  AutoCreateContext subCtxt;

  subCtxt->Inject<A>();

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

TEST_F(ScopeTest, AutowiringOrdering) {
  AutoCreateContext outer;
  CurrentContextPusher outerPshr(outer);
  AutoCreateContext inner1;
  AutoCreateContext inner2;
  AutoCreateContext inner3;
  AutoCreateContext inner4;

  // Autowire in outer context, AutoRequire in inner
  Autowired<A> a;
  {
    CurrentContextPusher pshr(inner1);

    AutoRequired<A> a2;
    EXPECT_FALSE(a.IsAutowired());
  }

  // AutoRequire in outer context, Autowire in inner
  AutoRequired<B> b;
  {
    CurrentContextPusher pshr(inner2);

    Autowired<B> b2;
    EXPECT_TRUE(b.IsAutowired());
    EXPECT_EQ(b->GetContext(), outer);
  }

  // AutoRequire in outer context, AutoRequire in inner
  AutoRequired<C> c;
  {
    CurrentContextPusher pshr(inner3);

    AutoRequired<C> c2;
    EXPECT_TRUE(c2.IsAutowired());
    EXPECT_NE(c->GetContext(), c2->GetContext());
  }

  // Autowire in outer context, Autowire in inner
  Autowired<D> d;
  {
    CurrentContextPusher pshr(inner4);

    Autowired<D> d2;

    // Verify preconditions and postconditions on autowiring:
    ASSERT_FALSE(d.IsAutowired());
    ASSERT_FALSE(d2.IsAutowired());
    AutoRequired<D> derp(m_create);

    ASSERT_TRUE(d.IsAutowired()) << "Outer scope autowired field failed to be instantiated on an element created in an interior scope";
    ASSERT_TRUE(d2.IsAutowired()) << "Interior scope field failed to be satisfied by a field initiated at an outer context";

    ASSERT_EQ(d->GetContext(), d2->GetContext());
    ASSERT_EQ(derp->GetContext(), d2->GetContext());
  }
}

