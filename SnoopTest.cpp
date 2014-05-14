// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "SnoopTest.h"
#include "TestFixtures/Decoration.h"

class UpBroadcastListener:
  public virtual EventReceiver
{
public:
  virtual void SimpleCall(void) {}
};

class SnoopTestBase:
  public virtual UpBroadcastListener,
  public ContextMember
{
public:
  SnoopTestBase(void):
    m_simpleCall(false),
    m_callCount(0)
  {}

  bool m_simpleCall;
  int m_callCount;

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

class SimpleEvent:
public virtual EventReceiver
{
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
    ctxt->Unsnoop(GetSelf<RemovesSelf>());
  }
public:
  RemovesSelf():
    counter(0)
  {};
  int counter;
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
    child->Snoop(parentMember);

    // Now fire an event from the child:
    AutoFired<UpBroadcastListener> firer;
    firer(&UpBroadcastListener::SimpleCall)();

    // Verify that the child itself got the message:
    EXPECT_TRUE(childMember->m_simpleCall) << "Message not received by another member of the same context";
  }

  // Verify that the parent got the message:
  EXPECT_TRUE(parentMember->m_simpleCall) << "Parent context snooper didn't receive a message broadcast by the child context";

  // Verify that the OTHER member got nothing:
  EXPECT_FALSE(ignored->m_simpleCall) << "A member in a parent context received a child-context message even though it didn't request to snoop that context";
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
    snoopy->Snoop(parentMember);
    snoopy->Unsnoop(parentMember);
    snoopy->Initiate();

    // Fire one event:
    AutoFired<UpBroadcastListener> firer;
    firer(&UpBroadcastListener::SimpleCall)();

    // The local listener should have gotten something
    EXPECT_TRUE(childMember->m_simpleCall) << "Message not received by a local listener after Unsnoop call";
  }

  EXPECT_FALSE(parentMember->m_simpleCall) << "ParentMember snooper received an event, even after an Unsnoop call was made";
}

TEST_F(SnoopTest, AmbiguousReciept) {
  AutoCurrentContext()->Initiate();
  AutoRequired<ParentMember> parent;

  // Fire and verify that disallow still receives the event:
  AutoFired<UpBroadcastListener> ubl;
  ASSERT_TRUE(ubl.HasListeners()) << "Expected at least one listener--the ParentMember instance";

  // Membership double-check:
  ASSERT_TRUE(m_create->IsMember(parent)) << "Could not find a just-autowired instance in the current context";

  {
    AutoCreateContext subCtxt;
    subCtxt->Snoop(parent);
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
  EXPECT_TRUE(parent->m_simpleCall) << "Snooped parent did not receive an event as expected when snooped context was destroyed";
}

TEST_F(SnoopTest, AvoidDoubleReciept) {
  AutoCreateContext sibCtxt;
  
  AutoCreateContext parentCtxt;
  CurrentContextPusher parentPshr(parentCtxt);
  
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
      child->Snoop(parentMember);
    }

    // Now fire an event from the parent:
    AutoFired<UpBroadcastListener> firer;
    firer(&UpBroadcastListener::SimpleCall)();

    // Verify that the child and parent got the message once
    EXPECT_EQ(1, childMember->m_callCount) << "Message not received by another member of the same context";
    EXPECT_EQ(1, parentMember->m_callCount) << "Parent context snooper didn't receive a message broadcast by the child context";
    
    
    // Test sibling context
    AutoRequired<SiblingMember> sibMember(sibCtxt);
    AutoRequired<SiblingMember> alsoInParent;
    child->Snoop(sibMember);
    
    firer(&UpBroadcastListener::SimpleCall)();
    
    EXPECT_EQ(2, childMember->m_callCount) << "Message not received by another member of the same context";
    EXPECT_EQ(2, parentMember->m_callCount) << "Parent context snooper didn't receive a message broadcast by the child context";
    EXPECT_EQ(1, alsoInParent->m_callCount) << "Parent context member didn't receive message";
    EXPECT_EQ(1, sibMember->m_callCount) << "Sibling context member didn't receive message";
    
    // Make sure unsnoop cleans up everything
    child->Unsnoop(sibMember);
    firer(&UpBroadcastListener::SimpleCall)();
    
    EXPECT_EQ(3, childMember->m_callCount) << "Message not received by another member of the same context";
    EXPECT_EQ(3, parentMember->m_callCount) << "Parent context snooper didn't receive a message broadcast by the child context";
    EXPECT_EQ(2, alsoInParent->m_callCount) << "Parent context member didn't receive message";
    EXPECT_EQ(1, sibMember->m_callCount) << "Sibling context member didn't unsnoop";
  }
}

TEST_F(SnoopTest, MultiSnoop) {
  AutoCurrentContext base;
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
  
  EXPECT_EQ(1, member->m_callCount) << "Received events from child contexts";
  
  // Snoop both
  ctxt1->Snoop(member);
  ctxt2->Snoop(member);
  base->Invoke(&UpBroadcastListener::SimpleCall)();
  ctxt1->Invoke(&UpBroadcastListener::SimpleCall)();
  ctxt2->Invoke(&UpBroadcastListener::SimpleCall)();
  
  EXPECT_EQ(4, member->m_callCount) << "Didn't receive all events";
  
  // Unsnoop one
  ctxt2->Unsnoop(member);
  base->Invoke(&UpBroadcastListener::SimpleCall)();
  ctxt1->Invoke(&UpBroadcastListener::SimpleCall)();
  ctxt2->Invoke(&UpBroadcastListener::SimpleCall)();
  
  EXPECT_EQ(6, member->m_callCount) << "Unsnooped both!";
  EXPECT_EQ(6, member1->m_callCount) << "Native context member didn't receive correct number of events";
  EXPECT_EQ(9, member2->m_callCount) << "Native context member didn't receive correct number of events";
}

TEST_F(SnoopTest, AntiCyclicRemoval) {
  AutoRequired<RemovesSelf> removeself;
  
  AutoCreateContext snoopy;
  CurrentContextPusher pshr(snoopy);
  snoopy->Initiate();
  
  snoopy->Snoop(removeself);
  
  ASSERT_EQ(0, removeself->counter);
  
  AutoFired<SimpleEvent> ubl;
  ubl(&SimpleEvent::ZeroArgs)();
  EXPECT_EQ(1, removeself->counter) << "Received event";
  
  ubl(&SimpleEvent::ZeroArgs)();
  EXPECT_EQ(1, removeself->counter) << "Received event even though unsnooped";
}


TEST_F(SnoopTest, SimplePackets) {
  AutoCreateContext Pipeline;
  AutoCreateContext Tracking;
  Pipeline->Initiate();
  Tracking->Initiate();
  
  // Add filter to tracking
  AutoRequired<FilterA> filter(Tracking);
  AutoRequired<FilterF> detachedFilter(Tracking);
  Pipeline->Snoop(filter);
  ASSERT_FALSE(filter->m_called) << "Filter called prematurely";
  ASSERT_FALSE(detachedFilter->m_called) << "Filter called prematurely";
  
  // Add factory to pipeline
  AutoRequired<AutoPacketFactory> factory(Pipeline);
  auto packet = factory->NewPacket();
  
  packet->Decorate(Decoration<0>());
  ASSERT_FALSE(filter->m_called) << "Filter called prematurely";
  
  // Now compleletly satisfy filter. Should snoop across contexts
  packet->Decorate(Decoration<1>());
  EXPECT_TRUE(filter->m_called) << "Snoop didn't work";
  EXPECT_FALSE(detachedFilter->m_called) << "Received a packet from a different context";
  
  //reset
  filter->m_called = false;
  
  Pipeline->Unsnoop(filter);
  auto packet2 = factory->NewPacket();
  packet2->Decorate(Decoration<0>());
  packet2->Decorate(Decoration<1>());
  EXPECT_FALSE(filter->m_called) << "Unsnoop didn't work";
}

