// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
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

