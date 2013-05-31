// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
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
  EXPECT_TRUE(global.IsAutowired()) << "Failed to autowire the global context";
}
