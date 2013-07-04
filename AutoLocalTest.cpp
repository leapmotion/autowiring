#include "stdafx.h"
#include "AutoLocalTest.h"
#include "CoreContext.h"

class StrictlyLocal {
};

TEST_F(AutoLocalTest, VerifyNegativeAutowiring) {
  AutoRequired<StrictlyLocal> exterior;

  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);

    // Attempt a local autowiring on a type that doesn't exist:
    AutowiredLocal<StrictlyLocal> local;
    EXPECT_FALSE(local.IsAutowired()) << "Locally autowired a type which was not local to the current context";
  }
}

TEST_F(AutoLocalTest, VerifyNoExteriorDeferred) {
  // Create a subcontext first:
  AutoCreateContext ctxt;
  CurrentContextPusher pshr(ctxt);

  // Attempt to autowire locally to this context--should fail:
  AutowiredLocal<StrictlyLocal> local;
  ASSERT_FALSE(local.IsAutowired());

  // Back out, and verify that an exterior AutoRequired type doesn't cause the local slot to receive a pointer:
  pshr.Pop();
  AutoRequired<StrictlyLocal> created;
  EXPECT_FALSE(local.IsAutowired()) << "A creation event unexpectedly satisfied a local pointer in a child context";
}

TEST_F(AutoLocalTest, VerifyNoLocalAliasing) {
  AutoRequired<StrictlyLocal> nonlocal;

  AutoCreateContext ctxt;
  CurrentContextPusher pshr(ctxt);

  AutoRequiredLocal<StrictlyLocal> local;
  EXPECT_TRUE(local.IsAutowired()) << "An AutoRequiredLocal instance was not autowired as expected";
  EXPECT_NE(nonlocal, local) << "A local instance aliased a type in an enclosing scope";
}