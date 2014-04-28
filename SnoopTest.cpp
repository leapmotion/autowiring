// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "SnoopTest.h"

class UpBroadcastListener:
  public virtual EventReceiver
{
public:
  virtual void SimpleCall(void) {}
};

class SnoopTestBase:
  public UpBroadcastListener
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
{
};

class IgnoredParentMember:
  public SnoopTestBase
{
};


class SimpleEvent:
public virtual EventReceiver
{
public:
  virtual void ZeroArgs(void) {}
};

class RemovesSelf:
public SimpleEvent,
public std::enable_shared_from_this<RemovesSelf>
{
  virtual void ZeroArgs(void){
    AutoCurrentContext ctxt;
    ctxt->Unsnoop(shared_from_this());
  }
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
  snoopy->Initiate();
  AutoRequired<ParentMember> parentMember;

  // Add a member to be snooped:
  {
    CurrentContextPusher pshr(snoopy);
    AutoRequired<ChildMember> childMember;

    // Snoop, unsnoop:
    snoopy->Snoop(parentMember);
    snoopy->Unsnoop(parentMember);

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

    // Verify that the child itself got the message
    EXPECT_EQ(1, childMember->m_callCount) << "Message not received by another member of the same context";
  }

  // Verify that the parent got the message only once:
  EXPECT_EQ(1, parentMember->m_callCount) << "Parent context snooper didn't receive a message broadcast by the child context";
}

TEST_F(SnoopTest, AntiCyclicRemoval) {
  AutoRequired<RemovesSelf> removeself;
  
  AutoCreateContext snoopy;
  CurrentContextPusher pshr(snoopy);
  snoopy->Initiate();
  
  snoopy->Snoop(removeself);
  
  AutoFired<SimpleEvent> ubl;
  ubl(&SimpleEvent::ZeroArgs)();
}
