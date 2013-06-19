#include "stdafx.h"
#include "TransientContextMemberTest.h"
#include "TransientContextMember.h"

class TransientClassA:
  public TransientContextMember
{
};

TEST_F(TransientContextMemberTest, VerifyTransience) {
  std::weak_ptr<TransientClassA> transWeak;

  {
    AutoContextCreator ctxt;
    CurrentContextPusher pshr(ctxt);

    AutoTransient<TransientClassA> trans;
    transWeak = trans;
  }

  EXPECT_TRUE(transWeak.expired());
}