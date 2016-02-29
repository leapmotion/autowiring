// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/autowiring.h>
#include <autowiring/CoreContext.h>
#include <autowiring/CurrentContextPusher.h>
#include <thread>

class CurrentContextPusherTest:
  public testing::Test
{};

using namespace std;

TEST_F(CurrentContextPusherTest, VerifyResetsDuringThrow) {
  std::shared_ptr<CoreContext> current = CoreContext::CurrentContext();

  try {
    AutoCreateContext subContext(current);
    CurrentContextPusher test(subContext.get());
    throw exception();
  } catch(exception&) {
  }

  ASSERT_EQ(current.get(), CoreContext::CurrentContext().get());
}

TEST_F(CurrentContextPusherTest, NoUnexpectedGlobalHold) {
  AutoCreateContext ctxt;
  AutoGlobalContext global;

  int initUses = global.use_count();
  std::thread rs(
    [&ctxt] {
      CurrentContextPusher pshr(ctxt);
    }
  );
  rs.join();
  ASSERT_TRUE(ctxt.unique()) << "The current context pointer was not correctly cleaned up on thread exit";
  ASSERT_EQ(initUses, global.use_count()) << "A global reference was unexpectedly leaked by the pusher";
}
