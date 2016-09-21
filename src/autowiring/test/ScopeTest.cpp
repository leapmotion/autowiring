// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/SimpleObject.hpp"
#include <autowiring/autowiring.h>
#include <autowiring/GlobalCoreContext.h>

class ScopeTest:
  public testing::Test
{};

TEST_F(ScopeTest, VerifyGlobalExists) {
  // Verify that we at least get a global scope
  AutoGlobalContext global;
  ASSERT_TRUE(nullptr != global.get()) << "Failed to autowire the global context";
}

class A : public ContextMember {};
class B : public ContextMember {};
class C : public ContextMember {};
class D : public ContextMember {};

TEST_F(ScopeTest, VerifyInherit) {
  AutoCurrentContext ctxt;

  // Add a member to the current context
  ctxt->Inject<A>();

  // Create and switch to a sub-context
  AutoCreateContext pContext(ctxt);
  {
    CurrentContextPusher pusher(pContext);
    ASSERT_NE(ctxt.get(), pContext.get()) << "Failed to create a sub-context";

    //try and autowire a member from the parent context
    Autowired<A> autoA;
    ASSERT_TRUE(autoA) << "Autowired member not wired from parent context";

    //add a member in the subcontext
    pContext->Inject<B>();
  }

  Autowired<B> autoB;
  ASSERT_FALSE(autoB.get()) << "Autowired member wired from sub-context";
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

  ctxt->Inject<NoSimpleConstructor>(10);

  Autowired<NoSimpleConstructor> wired;

  ASSERT_TRUE(wired.IsAutowired());
  ASSERT_EQ(10, wired->value);
}

TEST_F(ScopeTest, StaticInject){
  Autowired<A> preA;
  Autowired<B> preB;

  ASSERT_FALSE(preA.IsAutowired());
  ASSERT_FALSE(preB.IsAutowired());

  CoreContext::InjectCurrent<A>();
  CoreContext::InjectCurrent<B>();

  Autowired<A> a;
  Autowired<B> b;

  ASSERT_TRUE(a.IsAutowired());
  ASSERT_TRUE(b.IsAutowired());
}

TEST_F(ScopeTest, VerifyAutowireSpecifiedContext){
  AutoCurrentContext ctxt;

  AutoCreateContext subCtxt;

  subCtxt->Inject<A>();

  Autowired<A> aWired(subCtxt);
  ASSERT_TRUE(aWired) << "Autowired member not wired from the passed context";
}

TEST_F(ScopeTest, VerifyAutoRequireSpecifiedContext){
  AutoCreateContext subCtxt;
  AutoRequired<SimpleObject> aReq(subCtxt);
  Autowired<SimpleObject> aWired(subCtxt);
  Autowired<SimpleObject> aFail;

  ASSERT_TRUE(aWired) << "Autorequired member not added to the passed context";
  ASSERT_FALSE(aFail) << "Autorequired member added to the wrong context";
}

//This mangles the heap! why??  Using SimpleObject instead of A works fine!
//This beahvior was NOT introduced by the new context argument.
TEST_F(ScopeTest, AutowiringHeapMangle){
  AutoRequired<A> creator;
  Autowired<A> reference;
}

TEST_F(ScopeTest, AutowiringOrdering) {
  AutoCurrentContext ctxt;
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
    ASSERT_FALSE(a.IsAutowired());
  }

  // AutoRequire in outer context, Autowire in inner
  AutoRequired<B> b;
  {
    CurrentContextPusher pshr(inner2);

    Autowired<B> b2;
    ASSERT_TRUE(b.IsAutowired());
    ASSERT_EQ(b->GetContext(), outer);
  }

  // AutoRequire in outer context, AutoRequire in inner
  AutoRequired<C> c;
  {
    CurrentContextPusher pshr(inner3);

    AutoRequired<C> c2;
    ASSERT_TRUE(c2.IsAutowired());
    ASSERT_NE(c->GetContext(), c2->GetContext());
  }

  // Autowire in outer context, Autowire in inner
  Autowired<D> d;
  {
    CurrentContextPusher pshr(inner4);

    Autowired<D> d2;

    // Verify preconditions and postconditions on autowiring:
    ASSERT_FALSE(d.IsAutowired());
    ASSERT_FALSE(d2.IsAutowired());
    AutoRequired<D> derp(ctxt);

    ASSERT_TRUE(d.IsAutowired()) << "Outer scope autowired field failed to be instantiated on an element created in an interior scope";
    ASSERT_TRUE(d2.IsAutowired()) << "Interior scope field failed to be satisfied by a field initiated at an outer context";

    ASSERT_EQ(d->GetContext(), d2->GetContext());
    ASSERT_EQ(derp->GetContext(), d2->GetContext());
  }
}

class InnerContext {};
class MiddleContext {};
TEST_F(ScopeTest, RequireVsWire) {
  AutoCurrentContext ctxt_outer;
  auto ctxt_middle = ctxt_outer->Create<MiddleContext>();
  auto ctxt_inner = ctxt_middle->Create<InnerContext>();

  CurrentContextPusher pshr(ctxt_inner);

  Autowired<A> a_wired_inner;
  ASSERT_FALSE(a_wired_inner.IsAutowired()) << "Autowired member became autowired too quickly";

  AutoRequired<A> a_required_outer(ctxt_outer);
  ASSERT_TRUE(a_required_outer.IsAutowired()) << "AutoRequired member unsatisfied after construction";
  ASSERT_EQ(a_required_outer->GetContext(), ctxt_outer) << "AutoRequired member satisfied in the wrong context";

  ASSERT_TRUE(a_wired_inner.IsAutowired()) << "AutoRequired member in parent did not satisfy Autowired member in child";
  ASSERT_EQ(a_wired_inner.get(), a_required_outer.get()) << "Mismatch between Autowired and Autorequired members";

  //this overrides the slot in the middle context;
  AutoRequired<A> a_required_middle(ctxt_middle);
  ASSERT_TRUE(a_required_middle) << "AutoRequired member not satisfied!";
  ASSERT_EQ(ctxt_middle, a_required_middle->GetContext()) << "AutoRequired member not created in child context";
  ASSERT_NE(a_required_middle, a_required_outer) << "AutoRequired member not constructed in child context";
  
  Autowired<A> a_wired_inner2(ctxt_inner);
  ASSERT_TRUE(a_wired_inner2.IsAutowired());
  ASSERT_EQ(a_wired_inner2.get(), a_required_middle.get()) << "Autowired member did not redirect to the closest context";
  ASSERT_EQ(a_wired_inner.get(), a_required_outer.get()) << "Autowired object changed after satisfaction!";

  AutoRequired<B> b_required_middle(ctxt_middle);
  AutoRequired<B> b_required_outer(ctxt_outer);
  Autowired<B> b_wired_inner;
  ASSERT_TRUE(b_wired_inner.IsAutowired()) << "Autowired failed to find member";
  ASSERT_EQ(b_wired_inner.get(), b_required_middle.get()) << "Autorequiring overwrote a slot it shouldn't have!";
}

TEST_F(ScopeTest, RequireVsWireFast) {
  AutoCurrentContext ctxt_outer;
  auto ctxt_middle = ctxt_outer->Create<MiddleContext>();
  auto ctxt_inner = ctxt_middle->Create<InnerContext>();

  CurrentContextPusher pshr(ctxt_inner);

  //This adds a memo for this type to the inner context
  AutowiredFast<A> a_wired(ctxt_inner);
  ASSERT_FALSE(a_wired.IsAutowired()) << "Autowired member became autowired too quickly";

  //This satisfies the memo in the inner context
  AutoRequired<A> a_required_outer(ctxt_outer);
  ASSERT_TRUE(a_required_outer.IsAutowired()) << "AutoRequired member unsatisfied after construction";
  ASSERT_EQ(a_required_outer->GetContext(), ctxt_outer) << "AutoRequired member satisfied in the wrong context";

  ASSERT_FALSE(a_wired.IsAutowired()) << "AutowiredFast was satisfied after construction!";

  //This overrides the memo in the middle context.
  AutoRequired<A> a_required_middle(ctxt_middle);
  ASSERT_TRUE(a_required_middle.IsAutowired()) << "AutoRequired member not satisfied!";
  ASSERT_EQ(a_required_middle->GetContext(), ctxt_middle) << "AutoRequired member not created in child context";
  ASSERT_NE(a_required_middle.get(), a_required_outer.get()) << "AutoRequired member not constructed in child context";

  //This should direct to the middle context
  AutowiredFast<A> a_wired2(ctxt_inner);
  ASSERT_TRUE(a_wired2.IsAutowired());
  ASSERT_EQ(a_wired2, a_required_middle) << "Autowired member did not grab the most derived object";
}

class ParentInjector : public ContextMember {
public:
  ParentInjector(int v, bool inject = false) : ContextMember("Parent Injector"), value(v) {
    if (!inject) {
      return;
    }

    const auto ctxt = m_context.lock();
    if (!ctxt) {
      return;
    }

    auto parent = ctxt->GetParentContext();
    if (!parent) {
      return;
    }

    parent->Inject<ParentInjector>(9000, false);
  }
  int value;
};

TEST_F(ScopeTest, RecursiveInject) {
  AutoCurrentContext ctxt_outer;
  auto ctxt_inner = ctxt_outer->Create<InnerContext>();

  CurrentContextPusher pshr(ctxt_inner);

  AutoRequired<ParentInjector> a_inner(ctxt_inner, 1, true);

  ASSERT_EQ(a_inner->GetContext(), ctxt_inner);
  ASSERT_EQ(a_inner->value, 1);

  AutoRequired<ParentInjector> a_inner_2(ctxt_inner, 2);

  ASSERT_EQ(a_inner, a_inner_2);
  ASSERT_EQ(a_inner->GetContext(), a_inner_2->GetContext());
}

TEST_F(ScopeTest, NoPostHocAfterReset) {
  Autowired<SimpleObject> sobj;
  sobj.reset();
  AutoRequired<SimpleObject>();
  ASSERT_FALSE(sobj.IsAutowired()) << "An autowired slot was incorrectly satisfied after having been reset";
}
