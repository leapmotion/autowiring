// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoAnchorTest.hpp"
#include "TestFixtures/SimpleObject.hpp"

struct AnchorsNothing:
  AutoAnchor<>
{};

TEST_F(AutoAnchorTest, VerifyBaseCase) {
  AutoCreateContextT<AnchorsNothing> ctxt;
  ASSERT_TRUE(!!ctxt.get()) << "Failed to create a context which anchors nothing";
}

struct AnchorSigil:
  AutoAnchor<SimpleObject>
{};

TEST_F(AutoAnchorTest, VerifySimpleAnchor) {
  // Create a new context with the AnchorSigil sigil type, and then make it current
  AutoCreateContextT<AnchorSigil> ctxt;

  // Verify the sigil type
  ASSERT_EQ(typeid(AnchorSigil), ctxt->GetSigilType()) << "Anchor sigil context did not have the correct sigil type";

  // Now create a subcontext from here:
  AutoCreateContext subContext(ctxt);
  CurrentContextPusher pshr(subContext);

  // Verify parent-child relationship:
  ASSERT_EQ(subContext->GetParentContext().get(), ctxt.get()) << "A created subcontext was not a child of the specified parent context";

  // Try to create an object in the new child context:
  AutoRequired<SimpleObject> sobj;

  // Membership verification:
  ASSERT_EQ(ctxt, sobj->GetContext()) << "An anchored type was not injected in the context where it was anchored";
}

TEST_F(AutoAnchorTest, VerifyPostHocSatisfaction) {
  AutoCreateContextT<AnchorSigil> ctxt;
  AutoCreateContext subContext(ctxt);
  CurrentContextPusher pshr(subContext);

  // Create a slot in the child context:
  Autowired<SimpleObject> slot;
  ASSERT_FALSE(slot) << "A slot was satisfied prematurely";

  // Now try to autorequire--this will be satisfied in the anchor:
  AutoRequired<SimpleObject> sobj;

  // Verify that the slot was satisfied
  ASSERT_TRUE(slot.IsAutowired()) << "A slot in a dependent context was not correctly satisfied by the injection of an anchored type in an ancestor";

  // Verify equivalence:
  ASSERT_EQ(sobj.get(), slot.get()) << "A satisfied dependent context slot was not satisfied by the same instance injected by anchor";

  // Verify membership of the anchored type in the associated anchored context:
  ASSERT_EQ(ctxt, sobj->GetContext()) << "An anchored type with an unsatisfied slot in a child context was not injected in the context where it was anchored";
}

struct Foo{};

struct AnchorsOntoFoo:
  ContextMember,
  AutoAnchor<Foo>
{};

TEST_F(AutoAnchorTest, VerifyMemberBolting) {
  AutoCreateContextT<Foo> anchorCtxt;
  auto subCtxt = anchorCtxt->Create<void>();
  
  CurrentContextPusher pshr(subCtxt);
  
  AutoRequired<SimpleObject> simple;
  AutoRequired<AnchorsOntoFoo> anchors;
  
  EXPECT_EQ(subCtxt, simple->GetContext()) << "Normal AutoRequired ContextMember wasn't injected into the current context";
  EXPECT_EQ(anchorCtxt, anchors->GetContext()) << "Anchor wasn't resolved correctly for anchoring ContextMember";
}
