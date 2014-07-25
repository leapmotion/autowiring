// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "CurrentContextPusherTest.h"
#include "CoreContext.h"
#include "Autowired.h"
#include "CurrentContextPusher.h"

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
