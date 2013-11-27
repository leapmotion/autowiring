#include "stdafx.h"
#include "SimpleOwnershipValidatorTest.h"

TEST_F(SimpleOwnershipValidatorTest, VerifySimpleExclusion) {
  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);
    ctxt->EnforceSimpleOwnership();

    // Add an arbitrary object to this context:
  }
}