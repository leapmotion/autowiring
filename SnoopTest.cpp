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
    m_simpleCall(false)
  {}

  bool m_simpleCall;

  void SimpleCall(void) override {
    m_simpleCall = true;
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

class Disallowed:
  public ContextMember,
  public UpBroadcastListener
{
};

class DisallowedGeneric:
  public UpBroadcastListener
{
};

TEST_F(SnoopTest, VerifySimpleSnoop) {
  // Create the parent listener:
  AutoRequired<ParentMember> parentMember;

  // This listener instance shouldn't get any messages:
  AutoRequired<IgnoredParentMember> ignored;

  {
    // Create the child context and insert the child member:
    AutoCreateContext child;
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

    // Fire one event:
    AutoFired<UpBroadcastListener> firer;
    firer(&UpBroadcastListener::SimpleCall)();

    // The local listener should have gotten something
    EXPECT_TRUE(childMember->m_simpleCall) << "Message not received by a local listener after Unsnoop call";
  }

  EXPECT_FALSE(parentMember->m_simpleCall) << "ParentMember snooper received an event, even after an Unsnoop call was made";
}

TEST_F(SnoopTest, DetectDisallowedContextMember) {
  // Create two child contexts:
  AutoCreateContext sibling1;
  AutoCreateContext sibling2;

  // Create a member of the first context, and try to use it to snoop the second context:
  {
    CurrentContextPusher pshr(sibling1);
    AutoRequired<Disallowed> disallow;

    EXPECT_THROW(sibling2->Snoop(disallow), std::runtime_error);
  }
}

TEST_F(SnoopTest, DetectDisallowedGeneralType) {
  // Create two child contexts:
  AutoCreateContext sibling1;
  AutoCreateContext sibling2;

  // Create a member again, but this time, use the generic type so we can't use a context membership check
  {
    CurrentContextPusher pshr(sibling1);
    AutoRequired<DisallowedGeneric> disallowGeneric;

    EXPECT_THROW(sibling2->Snoop(disallowGeneric), std::runtime_error);
  }
}
