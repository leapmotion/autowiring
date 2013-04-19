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
  shared_ptr<SimpleThreaded> threaded;
  weak_ptr<CoreContext> weakContext;

  {
    Autowired<CoreContext> context;
    context.Create();

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