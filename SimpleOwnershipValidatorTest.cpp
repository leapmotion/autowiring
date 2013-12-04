#include "stdafx.h"
#include "SimpleOwnershipValidatorTest.h"
#include "TestFixtures/SimpleObject.h"

using namespace std;

TEST_F(SimpleOwnershipValidatorTest, VerifyExclusionCases) {
  bool hit = false;
  int count = -1;
  auto fn = 
    [&hit, &count](const std::vector<const type_info*>& violating) {
      hit = true;
      count = (int)violating.size();
    };

  std::shared_ptr<SimpleObject> obj(new SimpleObject);
  std::weak_ptr<SimpleObject> objWeak(obj);
  std::weak_ptr<CoreContext> ctxtWeak;
  {
    AutoCreateContext ctxt;
    ctxt->EnforceSimpleOwnership();
    ctxt->AddValidationListener(fn);
    ctxtWeak = ctxt;

    // Add an arbitrary object to this context:
    ctxt->Add(obj);
  }

  // Verify we got hit, at a minimum:
  ASSERT_TRUE(ctxtWeak.expired()) << "Context did not expire when it went out of scope as expected";
  ASSERT_TRUE(hit) << "Validation listener was not invoked, ownership check was not enforced";
  ASSERT_EQ(1UL, count) << "An insufficient number of entities were detected as being incorrectly owned";

  // Reset values, try again with another context:
  hit = false;
  count = -1;

  {
    AutoCreateContext ctxt;
    ctxt->EnforceSimpleOwnership();
    ctxt->AddValidationListener(fn);
    ctxtWeak = ctxt;

    // Add an arbitrary object to this context, but release the pointer this time:
    ctxt->Add(obj);
    obj.reset();
  }

  // Now verify that the object was destroyed as expected:
  ASSERT_TRUE(ctxtWeak.expired()) << "Context did not expire when it went out of scope as expected";
  ASSERT_TRUE(objWeak.expired()) << "Weak pointer to enclosure object was not cleaned up correctly";
  ASSERT_NE(0UL, count) << "A validation listener was invoked on a list of size zero, this is an error";
  ASSERT_EQ(-1, count) << "A validation listener was invoked even though it should not have been called";
  ASSERT_FALSE(hit) << "A validation listener was invoked when it should not have been invoked";
}