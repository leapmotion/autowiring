// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "ScopeTest.h"
#include "GlobalCoreContext.h"
#include "Autowired.h"

TEST_F(ScopeTest, VerifyGlobalExists) {
  // Verify that we at least get a global scope
  AutoGlobalContext global;
  EXPECT_TRUE(nullptr != global.get()) << "Failed to autowire the global context";
}
