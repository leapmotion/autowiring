#include "stdafx.h"
#include "ContextMapTest.h"
#include "Autowired.h"
#include "ContextMap.h"
#include <string>

using namespace std;

TEST_F(ContextMapTest, VerifySimple) {
  ContextMap<string> mp;

  // Create a new context and add it to the map:
  {
    Autowired<CoreContext> context;
    context.Create();

    // Verify the reference count or the rest of the test will fail
    ASSERT_EQ(context.use_count(), 1) << "A newly created context's use count isn't what was expected";
    
    // Add and ensure the reference count is unmodified
    mp.Add("context1", context);
    ASSERT_EQ(context.use_count(), 1) << "The map altered the context use count";

    // We should be able to find this context now:
    cpp11::shared_ptr<CoreContext> found = mp.Find("context1");
    EXPECT_TRUE(found) << "Failed to find a context that was just inserted into a context map";
  }

  // We shouldn't be able to find it now that it's gone out of scope:
  cpp11::shared_ptr<CoreContext> notFound = mp.Find("context1");
  EXPECT_FALSE(notFound) << "Context was not evicted as expected when it went out of scope";
}