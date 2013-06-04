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

class Firer:
  public EventManager<UpBroadcastListener>
{
public:
  void DoFire(void) {
    Fire(&UpBroadcastListener::SimpleCall)();
  };
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
    AutoRequired<Firer> firer;
    firer->DoFire();

    // Verify that the child itself got the message:
    EXPECT_TRUE(childMember->m_simpleCall) << "Message not received by another member of the same context";
  }

  // Verify that the parent got the message:
  EXPECT_TRUE(parentMember->m_simpleCall) << "Parent context snooper didn't receive a message broadcast by the child context";

  // Verify that the OTHER member got nothing:
  EXPECT_FALSE(ignored->m_simpleCall) << "A member in a parent context received a child-context message even though it didn't request to snoop that context";
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