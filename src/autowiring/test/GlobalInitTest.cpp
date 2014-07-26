// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "GlobalInitTest.hpp"
#include "TestFixtures/SimpleObject.hpp"
#include <autowiring/Autowired.h>
#include <autowiring/GlobalCoreContext.h>

void GlobalInitTest::SetUp(void) {
  // Always drop the global context when tests are done
  GlobalCoreContext::Release();

  // Also release the current context, whatever it is.
  CoreContext::EvictCurrent();
}

void GlobalInitTest::TearDown(void) {
  std::weak_ptr<GlobalCoreContext> glbl = AutoGlobalContext();

  // Always drop the global context when tests are done
  GlobalCoreContext::Release();

  // Also release the current context, whatever it is.
  CoreContext::EvictCurrent();

  // Be absolutely sure the global context went away
  ASSERT_TRUE(glbl.expired()) << "Global context did not tear down as expected";
}

TEST_F(GlobalInitTest, VerifyGlobalExists) {
  // Verify that we at least get a global scope
  std::shared_ptr<GlobalCoreContext> global = GlobalCoreContext::Get();
  EXPECT_TRUE(!!global.get());

  // There should only be three references:  The one we have, the global
  // reference, and the thread-current reference
  EXPECT_EQ(global.use_count(), 3) << "Unexpected global use count after bare initialization";
}

struct Simple {
  AutoRequired<SimpleObject> m_simple;
};

TEST_F(GlobalInitTest, VerifySimpleContext) {
  // Set our global scope stuff:
  GlobalContextDesignation<Simple> d;

  // Obtain reference:
  std::shared_ptr<GlobalCoreContext> global = GlobalCoreContext::Get();
  EXPECT_TRUE(!!global.get());

  // Verify that initialization happened as we expected:
  Autowired<SimpleObject> simpleObj;
  EXPECT_TRUE(!!simpleObj.get());
}
