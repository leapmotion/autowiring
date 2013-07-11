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