// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/ExitRaceThreaded.hpp"
#include "TestFixtures/SimpleThreaded.hpp"
#include <autowiring/autowiring.h>
#include <autowiring/ContextMap.h>
#include <string>
#include THREAD_HEADER

class ContextMapTest:
  public testing::Test
{
};

using namespace std;

TEST_F(ContextMapTest, VerifySimple) {
  ContextMap<string> mp;
  std::weak_ptr<CoreContext> ctxtWeak;

  // Create a new context and add it to the map:
  {
    AutoCreateContext context;
    ctxtWeak = context;

    // Verify the reference count or the rest of the test will fail
    ASSERT_TRUE(context.unique()) << "A newly created context's use count isn't what was expected";

    // Add and ensure the reference count is unmodified
    mp.Add("context_simple", context);
    ASSERT_TRUE(context.unique()) << "The map altered the context use count";

    // We should be able to find this context now:
    std::shared_ptr<CoreContext> found = mp.Find("context_simple");
    ASSERT_TRUE(!!found.get()) << "Failed to find a context that was just inserted into a context map";
  }
  ASSERT_TRUE(ctxtWeak.expired()) << "Context was leaked after it should have gone out of scope";

  // We shouldn't be able to find it now that it's gone out of scope:
  std::shared_ptr<CoreContext> notFound = mp.Find("context_simple");
  ASSERT_FALSE(!!notFound.get()) << "Context was not evicted as expected when it went out of scope";
}

TEST_F(ContextMapTest, VerifyWithThreads) {
  AutoCurrentContext()->Initiate();

  ContextMap<string> mp;
  std::shared_ptr<SimpleThreaded> threaded;
  std::weak_ptr<CoreContext> weakContext;

  {
    AutoCreateContext context;

    // Obtain a weak pointer of our own, and add to the context:
    weakContext = context;
    mp.Add("context_withthreads", context);

    // Add a thread to hold the context open for awhile:
    threaded = context->Inject<SimpleThreaded>();

    // Start the context
    context->Initiate();
  }

  // Assert that the context still actually exists:
  ASSERT_TRUE(!weakContext.expired()) << "Simple thread exited before it was signalled to exit";

  {
    // Verify that we can still find the context while the thread is alive:
    std::shared_ptr<CoreContext> context = mp.Find("context_withthreads");
    ASSERT_TRUE(!!context.get()) << "Map evicted a context before expected";

    // Relock the weak context, verify that we get back the same pointer:
    ASSERT_EQ(weakContext.lock(), context) << "Mapped context pointer was not identical to a previously stored context pointer";

    // Terminate whole context, wait for it to respond
    context->SignalShutdown();
    context->Wait();
    ASSERT_EQ(1UL, context.use_count()) << "Context reference should have been unique after thread expiration";
  }

  // Release our threaded entity:
  ASSERT_EQ(1UL, threaded.use_count()) << "Thread was holding a self-reference even after context termination has completed";
  threaded.reset();
  ASSERT_TRUE(weakContext.expired()) << "Context still existed even after the last reference to it should have been gone";

  {
    // Verify that the context is gone now that everything in it has stopped running
    auto ctxt = mp.Find("context_withthreads");
    ASSERT_FALSE(ctxt) << "Context was not properly evicted from the map";

    // Just return early if the context was empty as we expected, the next part of this test is for diagnostics
    if(!ctxt)
      return;

    // Release the pointer so we aren't guilty of holding a reference to the very thing whose
    // destruction we are trying to assure.
    ctxt.reset();

    // Sleep for a little bit and run the verification again.  If the prior expectation fails,
    // but this one succeeds, it could be due to race conditions in CoreThread
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ctxt = mp.Find("context_withthreads");
    ASSERT_FALSE(ctxt) << "Context was not properly evicted even after waiting for a time to ensure eviction";
  }
}

TEST_F(ContextMapTest, ConcurrentDestructionTestPathological) {
  for(size_t i = 0; i < 100; i++) {
    // Create our map and a few contexts:
    ContextMap<string> mp;
    AutoCreateContext contexts[4];
    weak_ptr<SimpleThreaded> threads[4];

    // Insert into the map:
    mp.Add("pathological_destruction0", contexts[0]);
    mp.Add("pathological_destruction1", contexts[1]);
    mp.Add("pathological_destruction2", contexts[2]);
    mp.Add("pathological_destruction3", contexts[3]);

    // Add a thread and kick off the context:
    for(size_t i = ARRAYCOUNT(contexts); i--;) {
      threads[i] = contexts[i]->Inject<SimpleThreaded>();
      contexts[i]->Initiate();
    }

    // Immediately tear contexts down:
    for(size_t i = ARRAYCOUNT(contexts); i--;)
      contexts[i]->SignalShutdown();

    // Wait on anything not signalled:
    for(size_t i = 0; i < ARRAYCOUNT(threads); i++) {
      auto cur = threads[i].lock();
      if(cur)
        ASSERT_TRUE(cur->WaitFor(std::chrono::seconds(1))) << "Spawned thread did not exit in a timely fashion";
    }
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
    context->Initiate();
  }

  // Set the signal:
  signal->Signal();

  // Verify that the map empties once our zero-count is hit:
  for(size_t i = 0; i < contexts.size(); i++) {
    contexts[i]->SignalShutdown(true);
  }

  // Clear the context collection:
  contexts.clear();
  ASSERT_EQ(0UL, mp.size()) << "Context map did not empty as expected";
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

  mp.Add("context_se_1", ctxt1);
  mp.Add("context_se_2", ctxt2);
  mp.Add("context_se_3", ctxt3);

  std::set<std::string> found;

  size_t count = 0;
  mp.Enumerate(
    [&count, &found] (const string& name, std::shared_ptr<CoreContext>& ctxt) -> bool {
      count++;
      found.insert(name);
      return true;
    }
  );

  ASSERT_EQ(3UL, count) << "Failed to enumerate all expected context pointers";
  ASSERT_EQ(1UL, found.count("context_se_1")) << "Failed to find map element '1'";
  ASSERT_EQ(1UL, found.count("context_se_2")) << "Failed to find map element '2'";
  ASSERT_EQ(1UL, found.count("context_se_3")) << "Failed to find map element '3'";
}

TEST_F(ContextMapTest, VerifyRangeBasedEnumeration) {
  std::shared_ptr<CoreContext> ctxt1 = AutoCreateContext();
  std::shared_ptr<CoreContext> ctxt2 = AutoCreateContext();
  std::shared_ptr<CoreContext> ctxt3 = AutoCreateContext();

  ContextMap<string> mp;
  mp.Add("a", ctxt1);
  mp.Add("b", ctxt2);
  mp.Add("c", ctxt3);

  size_t ct = 0;

  // Internal map in ContextMap ensures entries are enumerated in-order
  for (auto& cur : mp) {
    (void)cur;
    switch (ct) {
    case 0:
      // Release the entry we are presently enumerating
      ctxt1.reset();
      break;
    case 1:
      // No behavior
      break;
    case 2:
      // Release an entry we already enumerated
      ctxt2.reset();
      break;
    }
    ct++;
  }

  ASSERT_EQ(3UL, ct) << "Context map range-based enumeration did not correctly enumerate all members";
}

TEST_F(ContextMapTest, Clear) {
  std::shared_ptr<CoreContext> ctxt1 = AutoCreateContext();
  std::shared_ptr<CoreContext> ctxt2 = AutoCreateContext();
  std::shared_ptr<CoreContext> ctxt3 = AutoCreateContext();

  ContextMap<string> mp;
  mp.Add("a", ctxt1);
  mp.Add("b", ctxt2);
  mp.Add("c", ctxt3);

  ASSERT_EQ(3UL, mp.size()) << "Map did not have the correct number of entries after setup";
  mp.clear();
  ASSERT_EQ(0UL, mp.size()) << "Map clear operation did not clear the map itself as expected";
}
