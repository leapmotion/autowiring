#include "stdafx.h"
#include "ContextMapTest.h"
#include "Autowired.h"
#include "ContextMap.h"
#include "TestFixtures/SimpleThreaded.h"
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

TEST_F(ContextMapTest, VerifyWithThreads) {
  ContextMap<string> mp;
  cpp11::shared_ptr<SimpleThreaded> threaded;
  cpp11::weak_ptr<CoreContext> weakContext;

  {
    Autowired<CoreContext> context(true);

    // Obtain a weak pointer of our own, and add to the context:
    weakContext = context;
    mp.Add("context1", context);

    // Add a thread to hold the context open for awhile:
    threaded = context->Add<SimpleThreaded>();

    // Start the context
    context->InitiateCoreThreads();
  }

  // Assert that the context still actually exists:
  ASSERT_TRUE(!weakContext.expired()) << "Simple thread exited before it was signalled to exit";

  {
    // Verify that we can still find the context while the thread is alive:
    cpp11::shared_ptr<CoreContext> context = mp.Find("context1");
    ASSERT_TRUE(context) << "Map evicted a context before expected";

    // Begin context shutdown
    context->SignalShutdown();

    // Signal that the thread can quit:
    {
      boost::lock_guard<boost::mutex> lk(threaded->m_condLock);
      threaded->m_cond.notify_all();
    }

    // Wait for the context to exit:
    context->Wait();
  }

  // Release our threaded entity:
  threaded.reset();

  {
    // Verify that we can still find the context while the thread is alive:
    cpp11::shared_ptr<CoreContext> notFound = mp.Find("context1");
    EXPECT_FALSE(notFound) << "Context was not properly evicted from the map";
  }
}

TEST_F(ContextMapTest, AdjacentCleanupTest) {
  ContextMap<string> mp;
  cpp11::weak_ptr<CoreContext> outerWeak;
  cpp11::weak_ptr<CoreContext> innerWeak;

  // Add two contexts, and let one go out of scope
  Autowired<CoreContext> outer(true);
  mp.Add("0", outer);
  outerWeak = outer;

  {
    Autowired<CoreContext> inner(true);
    mp.Add("1", inner);
    innerWeak = inner;

    // Verify that we can find both contexts
    cpp11::shared_ptr<CoreContext> outerSearched = mp.Find("0");
    ASSERT_TRUE(outerSearched) << "Outer context just added, but couldn't be found";

    cpp11::shared_ptr<CoreContext> innerSearched = mp.Find("1");
    ASSERT_TRUE(innerSearched) << "Inner context just added, but couldn't be found";
  }

  // Inner should be 404 by now
  ASSERT_TRUE(innerWeak.expired()) << "Unexpected outstanding reference to the inner context";

  // Try to find the outer context.  This should evict the inner context.
  mp.Find("0");
  ASSERT_EQ(1UL, mp.size()) << "Proximity eviction didn't function as expected";
}