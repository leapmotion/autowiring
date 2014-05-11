#include "stdafx.h"
#include "AutowiringTest.h"
#include "Autowired.h"
#include "TestFixtures/SimpleObject.h"
#include "TestFixtures/SimpleReceiver.h"

TEST_F(AutowiringTest, VerifyAutowiredFast) {
  // Add an object:
  m_create->Inject<SimpleObject>();

  // Verify that AutowiredFast can find this object
  AutowiredFast<SimpleObject> sobj;
  ASSERT_TRUE(sobj.IsAutowired()) << "Failed to autowire an object which was just injected into a context";
}

TEST_F(AutowiringTest, VerifyAutowiredFastNontrivial) {
  // This will cause a cache entry to be inserted into the CoreContext's memoization system.
  // If there is any improper or incorrect invalidation in that system, then the null entry
  // will create problems when we attempt to perform an AutowiredFast later on.
  AutowiredFast<CallableInterface>();

  // Now we add the object
  AutoRequired<SimpleReceiver>();

  // Verify that AutowiredFast can find this object from its interface
  Autowired<CallableInterface> ci;
  ASSERT_TRUE(ci.IsAutowired()) << "Failed to autowire an interface advertised by a newly-inserted object";
}