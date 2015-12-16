// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"
#include "TestFixtures/SimpleObject.hpp"

class SnoopTest:
  public testing::Test
{};

class UpBroadcastListener {
public:
  virtual void SimpleCall(void) {}
};

class SnoopTestBase:
  public virtual UpBroadcastListener,
  public ContextMember
{
public:
  bool m_simpleCall = false;
  int m_callCount = 0;

  void SimpleCall(void) override {
    m_simpleCall = true;
    m_callCount++;
  }
};

class ChildMember:
  public SnoopTestBase
{};

class ParentMember:
  public SnoopTestBase
{};

class SiblingMember:
public SnoopTestBase
{};

class IgnoredParentMember:
  public SnoopTestBase
{};

class SimpleEvent {
public:
  virtual void ZeroArgs(void) {}
};

class RemovesSelf:
  public virtual SimpleEvent,
  public ContextMember
{
  virtual void ZeroArgs(void){
    counter++;
    AutoCurrentContext ctxt;
    ctxt->RemoveSnooper(GetSelf<RemovesSelf>());
  }
public:
  int counter = 0;
};

TEST_F(SnoopTest, VerifySimpleSnoop) {
  // Create the parent listener:
  AutoRequired<ParentMember> parentMember;

  // This listener instance shouldn't get any messages:
  AutoRequired<IgnoredParentMember> ignored;

  {
    // Create the child context and insert the child member:
    AutoCreateContext child;
    child->Initiate();
    CurrentContextPusher pshr(child);
    AutoRequired<ChildMember> childMember;

    // Snoop
    child->AddSnooper(parentMember);

    // Now fire an event from the child:
    AutoFired<UpBroadcastListener> firer;
    firer(&UpBroadcastListener::SimpleCall)();

    // Verify that the child itself got the message:
    ASSERT_TRUE(childMember->m_simpleCall) << "Message not received by another member of the same context";
  }

  // Verify that the parent got the message:
  ASSERT_TRUE(parentMember->m_simpleCall) << "Parent context snooper didn't receive a message broadcast by the child context";

  // Verify that the OTHER member got nothing:
  ASSERT_FALSE(ignored->m_simpleCall) << "A member in a parent context received a child-context message even though it didn't request to snoop that context";
}

TEST_F(SnoopTest, VerifyUnsnoop) {
  // Create a child context to snoop:
  AutoCreateContext snoopy;
  AutoRequired<ParentMember> parentMember;

  // Add a member to be snooped:
  {
    CurrentContextPusher pshr(snoopy);
    AutoRequired<ChildMember> childMember;

    // Snoop, unsnoop:
    snoopy->AddSnooper(parentMember);
    snoopy->RemoveSnooper(parentMember);
    snoopy->Initiate();

    // Fire one event:
    AutoFired<UpBroadcastListener> firer;
    firer(&UpBroadcastListener::SimpleCall)();

    // The local listener should have gotten something
    ASSERT_TRUE(childMember->m_simpleCall) << "Message not received by a local listener after Unsnoop call";
  }

  ASSERT_FALSE(parentMember->m_simpleCall) << "ParentMember snooper received an event, even after an Unsnoop call was made";
}

TEST_F(SnoopTest, AmbiguousReciept) {
  AutoCurrentContext()->Initiate();
  AutoRequired<ParentMember> parent;

  // Fire and verify that disallow still receives the event:
  AutoFired<UpBroadcastListener> ubl;
  ASSERT_TRUE(ubl.HasListeners()) << "Expected at least one listener--the ParentMember instance";

  {
    AutoCreateContext subCtxt;
    subCtxt->AddSnooper(parent);
    subCtxt->Initiate();

    // Verify that simple firing _here_ causes transmission as expected:
    AutoFired<UpBroadcastListener> ubl;
    ubl(&UpBroadcastListener::SimpleCall)();
    ASSERT_TRUE(parent->m_simpleCall) << "Snooped parent trivially failed to receive an event";

    // Reset the flag:
    parent->m_simpleCall = false;
  }

  // Should still be listeners at this point:
  ASSERT_TRUE(ubl.HasListeners()) << "Apparently no listeners exist after subcontext destruction";

  ubl(&UpBroadcastListener::SimpleCall)();
  ASSERT_TRUE(parent->m_simpleCall) << "Snooped parent did not receive an event as expected when snooped context was destroyed";
}

TEST_F(SnoopTest, AvoidDoubleReciept) {
  AutoCurrentContext()->Initiate();

  AutoCreateContext sibCtxt;
  
  AutoCreateContext parentCtxt;
  CurrentContextPusher parentPshr(parentCtxt);

  sibCtxt->Initiate();
  parentCtxt->Initiate();
  
  // Create the parent listener:
  AutoRequired<ParentMember> parentMember;
  {
    // Create the child context and insert the child member:
    std::shared_ptr<ChildMember> childMember;
    AutoCreateContext child;
    child->Initiate();
    {
      CurrentContextPusher pshr(child);
      childMember = child->Inject<ChildMember>();

      // Snoop
      child->AddSnooper(parentMember);
    }

    // Now fire an event from the parent:
    AutoFired<UpBroadcastListener> firer;
    firer(&UpBroadcastListener::SimpleCall)();

    // Verify that the child and parent got the message once
    ASSERT_EQ(1, childMember->m_callCount) << "Message not received by another member of the same context";
    ASSERT_EQ(1, parentMember->m_callCount) << "Parent context snooper didn't receive a message broadcast by the child context";
    
    
    // Test sibling context
    AutoRequired<SiblingMember> sibMember(sibCtxt);
    AutoRequired<SiblingMember> alsoInParent;
    child->AddSnooper(sibMember);
    
    firer(&UpBroadcastListener::SimpleCall)();
    
    ASSERT_EQ(2, childMember->m_callCount) << "Message not received by another member of the same context";
    ASSERT_EQ(2, parentMember->m_callCount) << "Parent context snooper didn't receive a message broadcast by the child context";
    ASSERT_EQ(1, alsoInParent->m_callCount) << "Parent context member didn't receive message";
    ASSERT_EQ(1, sibMember->m_callCount) << "Sibling context member didn't receive message";
    
    // Make sure unsnoop cleans up everything
    child->RemoveSnooper(sibMember);
    firer(&UpBroadcastListener::SimpleCall)();
    
    ASSERT_EQ(3, childMember->m_callCount) << "Message not received by another member of the same context";
    ASSERT_EQ(3, parentMember->m_callCount) << "Parent context snooper didn't receive a message broadcast by the child context";
    ASSERT_EQ(2, alsoInParent->m_callCount) << "Parent context member didn't receive message";
    ASSERT_EQ(1, sibMember->m_callCount) << "Sibling context member didn't unsnoop";
  }
}

TEST_F(SnoopTest, MultiSnoop) {
  AutoCurrentContext base;
  base->Initiate();

  auto ctxt1 = base->Create<int>();
  auto ctxt2 = ctxt1->Create<double>();

  AutoRequired<ParentMember> member(base);
  AutoRequired<ParentMember> member1(ctxt1);
  AutoRequired<ParentMember> member2(ctxt2);
  ASSERT_EQ(0, member->m_callCount);
  
  ctxt2->Initiate();
  
  // No Snoopers
  base->Invoke(&UpBroadcastListener::SimpleCall)();
  ctxt1->Invoke(&UpBroadcastListener::SimpleCall)();
  ctxt2->Invoke(&UpBroadcastListener::SimpleCall)();
  
  ASSERT_EQ(1, member->m_callCount) << "Received events from child contexts";
  member->m_callCount = 0;
  
  // Snoop both.  Invocation in an uninitialized context should not cause any handlers to be raised.
  ctxt1->AddSnooper(member);
  ctxt2->AddSnooper(member);
  base->Invoke(&UpBroadcastListener::SimpleCall)();
  ctxt1->Invoke(&UpBroadcastListener::SimpleCall)();
  ctxt2->Invoke(&UpBroadcastListener::SimpleCall)();
  
  ASSERT_EQ(2, member->m_callCount) << "Didn't receive all events";
  member->m_callCount = 0;
  member1->m_callCount = 0;
  member2->m_callCount = 0;
  
  // Unsnoop one
  ctxt2->RemoveSnooper(member);
  base->Invoke(&UpBroadcastListener::SimpleCall)();
  ctxt1->Invoke(&UpBroadcastListener::SimpleCall)();
  ctxt2->Invoke(&UpBroadcastListener::SimpleCall)();
  
  ASSERT_EQ(1, member->m_callCount) << "Unsnooped both!";
  ASSERT_EQ(0, member1->m_callCount) << "Native context member didn't receive correct number of events";
  ASSERT_EQ(1, member2->m_callCount) << "Native context member didn't receive correct number of events";
}

TEST_F(SnoopTest, AntiCyclicRemoval) {
  AutoRequired<RemovesSelf> removeself;
  
  AutoCreateContext snoopy;
  CurrentContextPusher pshr(snoopy);
  snoopy->Initiate();
  
  snoopy->AddSnooper(removeself);
  
  ASSERT_EQ(0, removeself->counter);
  
  AutoFired<SimpleEvent> ubl;
  ubl(&SimpleEvent::ZeroArgs)();
  ASSERT_EQ(1, removeself->counter) << "Received event";
  
  ubl(&SimpleEvent::ZeroArgs)();
  ASSERT_EQ(1, removeself->counter) << "Received event even though unsnooped";
}


TEST_F(SnoopTest, SimplePackets) {
  AutoCurrentContext()->Initiate();

  AutoCreateContext Pipeline;
  AutoCreateContext Tracking;
  Pipeline->Initiate();
  Tracking->Initiate();
  
  // Add filter to tracking
  AutoRequired<FilterA> filter(Tracking);
  AutoRequired<FilterF> detachedFilter(Tracking);
  Pipeline->AddSnooper(filter);
  ASSERT_FALSE(!!filter->m_called) << "Filter called prematurely";
  ASSERT_FALSE(detachedFilter->m_called) << "Filter called prematurely";

  // Add factory to pipeline
  AutoRequired<AutoPacketFactory> factory(Pipeline);
  auto packet = factory->NewPacket();

  packet->Decorate(Decoration<0>());
  ASSERT_FALSE(!!filter->m_called) << "Filter called prematurely";

  // Now compleletly satisfy filter. Should snoop across contexts
  packet->Decorate(Decoration<1>());
  ASSERT_TRUE(!!filter->m_called) << "A snooper did not receive an AutoPacket originating in a snooped context";
  ASSERT_FALSE(detachedFilter->m_called) << "Received a packet from a different context";

  //reset
  filter->m_called = false;
  Pipeline->RemoveSnooper(filter);
  auto packet2 = factory->NewPacket();
  packet2->Decorate(Decoration<0>());
  packet2->Decorate(Decoration<1>());
  ASSERT_FALSE(!!filter->m_called) << "Unsnoop didn't work";
}

TEST_F(SnoopTest, CanSnoopAutowired) {
  AutoCurrentContext ctxt;
  ctxt->Inject<SimpleObject>();

  // Now autowire what we injected and verify we can snoop this directly
  Autowired<SimpleObject> so;
  ctxt->AddSnooper(so);
}

TEST_F(SnoopTest, RuntimeSnoopCall) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  auto x = std::make_shared<ChildMember>();
  CoreObjectDescriptor traits(x);

  // Try to snoop and verify that the snooped member gets an event:
  ctxt->AddSnooper(x);

  AutoFired<UpBroadcastListener> ubl;
  ubl(&UpBroadcastListener::SimpleCall)();

  ASSERT_TRUE(x->m_simpleCall) << "Runtime snoop call did not correctly register a child context member";
  ASSERT_EQ(1UL, x->m_callCount) << "Call count to a child member was incorrect";

  // And the alternative variant next:
  ctxt->RemoveSnooper(x);
  ubl(&UpBroadcastListener::SimpleCall)();
  ASSERT_EQ(1UL, x->m_callCount) << "Snoop method was invoked after the related type was removed";
}
