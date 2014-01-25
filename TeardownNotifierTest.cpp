#include "stdafx.h"
#include "TeardownNotifierTest.h"
#include "Autowired.h"

class SimpleMember:
  public ContextMember
{
};

TEST_F(TeardownNotifierTest, VerifySingleNotification) {
  bool hit;
  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);

    AutoRequired<SimpleMember> member;
    member->AddTeardownListener([&hit] {hit = true;});
  }

  EXPECT_TRUE(hit) << "Teardown listener was not hit as expected during context teardown";
}

TEST_F(TeardownNotifierTest, ReferenceMemberInTeardown) {

  bool hit = false;
  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);

    AutoRequired <SimpleMember> member;

    std::weak_ptr<CoreContext> ctxtWeak;
    ctxt->AddTeardownListener[ctxtWeak] {
      try
      {
        Autowired<SimpleMember> member;
        if (member)
          hit = true;
      }
      catch (autowiring_error) {}
    });
  }

  EXPECT_TRUE(hit) << "Failed to reference a member of a context in it's teardown listener"
}