#include "stdafx.h"
#include "GlobalInitTest.h"
#include "GlobalCoreContext.h"
#include "Autowired.h"
#include "TestFixtures/SimpleObject.h"

void GlobalInitTest::SetUp(void) {
  // Always drop the global context when tests are done
  GlobalCoreContext::Release();

  // Also release the current context, whatever it is.
  CoreContext::EvictCurrent();
}

void GlobalInitTest::TearDown(void) {
  // Always drop the global context when tests are done
  GlobalCoreContext::Release();

  // Also release the current context, whatever it is.
  CoreContext::EvictCurrent();
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
