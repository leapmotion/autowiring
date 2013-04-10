#include "stdafx.h"
#include "ScopeTest.h"
#include "GlobalCoreContext.h"
#include "Autowired.h"

ScopeTest::ScopeTest(void)
{
}

ScopeTest::~ScopeTest(void)
{
}

TEST_F(ScopeTest, VerifyGlobalExists)
{
  // Verify that we at least get a global scope
  Autowired<GlobalCoreContext> global;
  EXPECT_TRUE(global);
}