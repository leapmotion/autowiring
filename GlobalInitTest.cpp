#include "stdafx.h"
#include "GlobalInitTest.h"
#include "GlobalCoreContext.h"
#include "Autowired.h"

// Simple ContextMember class:
class SimpleObject:
  public ContextMember
{
public:
  SimpleObject(void):
    zero(0),
    one(1)
  {}

  int zero;
  int one;
};

void GlobalInitTest::TearDown(void) {
  // Always drop the global context when tests are done
  GlobalCoreContext::Release();

  // Also release the current context, whatever it is.
  CoreContext::EvictCurrent();
}

TEST_F(GlobalInitTest, VerifyGlobalExists) {
  // Verify that we at least get a global scope
  cpp11::shared_ptr<GlobalCoreContext> global = GlobalCoreContext::Get();
  EXPECT_TRUE(global);

  // There should only be three references:  The one we have, the global
  // reference, and the thread-current reference
  EXPECT_EQ(global.use_count(), 3);
}

TEST_F(GlobalInitTest, VerifySimpleContext) {
  // Set our global scope stuff:
  struct Simple {
    AutoRequired<SimpleObject> m_simple;
  };
  GlobalCoreContext::AddGlobalObjects<Simple>();

  // Obtain reference:
  cpp11::shared_ptr<GlobalCoreContext> global = GlobalCoreContext::Get();
  EXPECT_TRUE(global);

  // Verify that initialization happened as we expected:
  Autowired<SimpleObject> simpleObj;
  EXPECT_TRUE(simpleObj);
}