// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "ContextMapTest.h"
#include "Autowired.h"
#include "ContextMap.h"
#include "TestFixtures/ExitRaceThreaded.h"
#include "TestFixtures/SimpleThreaded.h"
#include <string>

using namespace std;

TEST_F(ContextMapTest, VerifySimple) {
  ContextMap<string> mp;

  // Create a new context and add it to the map:
  {
    AutoCreateContext context;

    // Verify the reference count or the rest of the test will fail
    ASSERT_EQ(context.use_count(), 1) << "A newly created context's use count isn't what was expected";

    // Add and ensure the reference count is unmodified
    mp.Add("context1", context);
    ASSERT_EQ(context.use_count(), 1) << "The map altered the context use count";

    // We should be able to find this context now:
    std::shared_ptr<CoreContext> found = mp.Find("context1");
    EXPECT_TRUE(!!found.get()) << "Failed to find a context that was just inserted into a context map";
  }

  // We shouldn't be able to find it now that it's gone out of scope:
  std::shared_ptr<CoreContext> notFound = mp.Find("context1");
  EXPECT_FALSE(!!notFound.get()) << "Context was not evicted as expected when it went out of scope";
}

TEST_F(ContextMapTest, VerifyWithThreads) {
  ContextMap<string> mp;
  std::shared_ptr<SimpleThreaded> threaded;
  std::weak_ptr<CoreContext> weakContext;

  {
    AutoCreateContext context;

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
    std::shared_ptr<CoreContext> context = mp.Find("context1");
    ASSERT_TRUE(!!context.get()) << "Map evicted a context before expected";
    
    // Relock the weak context, verify that we get back the same pointer:
    auto relocked = weakContext.lock();
    EXPECT_EQ(relocked, context) << "Mapped context pointer was not identical to a previously stored context pointer";

    // Begin context shutdown
    context->SignalShutdown();

    // Signal that the thread can quit:
    (boost::lock_guard<boost::mutex>)threaded->m_condLock;
    threaded->m_cond.notify_all();

    // Wait for the context to exit:
    context->Wait();
  }

  // Release our threaded entity:
  threaded.reset();

  {
    // Verify that we can still find the context while the thread is alive:
    std::shared_ptr<CoreContext> notFound = mp.Find("context1");
    EXPECT_FALSE(notFound) << "Context was not properly evicted from the map";
  }
}

TEST_F(ContextMapTest, OutOfOrderDeletionTest) {
  try {
    AutoCreateContext controlled;
    ContextMap<size_t> mp;

    // Add the current context to the map:
    mp.Add(1, controlled);

    // Map is destroyed first, then the enclosed context--no exceptions should be thrown
  } catch(...) {
    FAIL() << "Exception thrown while attempting an out-of-order teardown";
  }
}

TEST_F(ContextMapTest, VerifyWithThreadsPathological) {
  ContextMap<size_t> mp;
  
  // Context collection and exit race threads:
  vector<std::shared_ptr<CoreContext>> contexts;

  // Exit race controller:
  AutoRequired<ExitRaceSignal> signal;

  // Create a number of dependent contexts:
  for(size_t i = 0; i < 100; i++) {
    AutoCreateContext context;
    contexts.push_back(context);

    // Store a shared pointer
    mp.Add(i, context);

    // Start the context
    context->InitiateCoreThreads();
  }

  // Set the signal:
  signal->Signal();

  // Verify that the map empties once our zero-count is hit:
  for(size_t i = 0; i < contexts.size(); i++)
    contexts[i]->Wait();
  
  // Clear the context collection:
  contexts.clear();
  EXPECT_EQ(0UL, mp.size()) << "Context map did not empty as expected";
}

TEST_F(ContextMapTest, AdjacentCleanupTest) {
  ContextMap<string> mp;
  std::weak_ptr<CoreContext> outerWeak;
  std::weak_ptr<CoreContext> innerWeak;

  // Add two contexts, and let one go out of scope
  AutoCreateContext outer;
  mp.Add("0", outer);
  outerWeak = outer;

  {
    AutoCreateContext inner;
    mp.Add("1", inner);
    innerWeak = inner;

    // Verify that we can find both contexts
    std::shared_ptr<CoreContext> outerSearched = mp.Find("0");
    ASSERT_TRUE(!!outerSearched.get()) << "Outer context just added, but couldn't be found";

    std::shared_ptr<CoreContext> innerSearched = mp.Find("1");
    ASSERT_TRUE(!!innerSearched.get()) << "Inner context just added, but couldn't be found";
  }

  // Inner should be 404 by now
  ASSERT_TRUE(innerWeak.expired()) << "Unexpected outstanding reference to the inner context";

  // Try to find the outer context.  This should evict the inner context.
  mp.Find("0");
  ASSERT_EQ(1UL, mp.size()) << "Proximity eviction didn't function as expected";
}

TEST_F(ContextMapTest, VerifySimpleEnumeration) {
  ContextMap<string> mp;
  AutoCreateContext ctxt1;
  AutoCreateContext ctxt2;
  AutoCreateContext ctxt3;

  mp.Add("1", ctxt1);
  mp.Add("2", ctxt2);
  mp.Add("3", ctxt3);

  size_t count = 0;
  mp.Enumerate(
    [&count] (const string&, std::shared_ptr<CoreContext>& ctxt) {
      count++;
    }
  );

  EXPECT_EQ(3UL, count) << "Failed to enumerate all expected context pointers";
}

