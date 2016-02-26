// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "GlobalInitTest.hpp"
#include "TestFixtures/SimpleObject.hpp"
#include <autowiring/autowiring.h>
#include <autowiring/GlobalCoreContext.h>

void GlobalInitTest::SetUp(void) {
  // Always drop the global context when tests are done
  GlobalCoreContext::Release();

  // Also release the current context, whatever it is.
  CoreContext::EvictCurrent();
}

void GlobalInitTest::TearDown(void) {
  std::shared_ptr<GlobalCoreContext> glbl = AutoGlobalContext();

  {
    // Always drop the global context when tests are done
    ASSERT_EQ(glbl, GlobalCoreContext::Release()) << "Release did not correctly return the previously current global context";
  }

  // Also release the current context, whatever it is.
  CoreContext::EvictCurrent();

  // Be absolutely sure the global context went away
  ASSERT_TRUE(glbl.unique()) << "An unexpected additional reference to the global context was detected";
}

TEST_F(GlobalInitTest, VerifyGlobalExists) {
  // Verify that we at least get a global scope
  std::shared_ptr<GlobalCoreContext> global = GlobalCoreContext::Get();
  ASSERT_TRUE(!!global.get());

  // There should only be three references:  The one we have, the global
  // reference, and the thread-current reference
  ASSERT_EQ(global.use_count(), 3) << "Unexpected global use count after bare initialization";
}
