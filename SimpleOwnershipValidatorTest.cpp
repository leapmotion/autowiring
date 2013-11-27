#include "stdafx.h"
#include "SimpleOwnershipValidatorTest.h"
#include "TestFixtures/SimpleObject.h"

TEST_F(SimpleOwnershipValidatorTest, VerifySimpleExclusion) {
  bool hit = false;
  size_t count = 0;

  std::shared_ptr<SimpleObject> obj;
  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);
    ctxt->EnforceSimpleOwnership();
    ctxt->AddValidationListener(
      [&hit, &count](const std::vector<const type_info*>& violating) {
        hit = true;
        count = violating.size();
      }
    );

    // Add an arbitrary object to this context:
    obj = AutoRequired<SimpleObject>();
  }

  // Verify we got hit, at a minimum:
  ASSERT_TRUE(hit) << "Validation listener was not invoked, ownership check was not enforced";
  ASSERT_EQ(1UL, count) << "An insufficient number of entities were detected as being incorrectly owned";
}