// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "ContextCleanupTest.h"
#include "Autowired.h"
#include "CoreContext.h"
#include "TestFixtures/SimpleObject.h"
#include "TestFixtures/SimpleThreaded.h"
#include <boost/chrono.hpp>

using boost::chrono::milliseconds;

TEST_F(ContextCleanupTest, ValidateTeardownOrder) {
  class WeakPtrChecker {
  public:
    ~WeakPtrChecker(void) {
      EXPECT_TRUE(self.expired()) << "Shared pointer to this was not expired at destructor time";
    }

    std::weak_ptr<WeakPtrChecker> self;
  };

  std::shared_ptr<WeakPtrChecker>(new WeakPtrChecker);
}

TEST_F(ContextCleanupTest, VerifyNoEarlyDtor) {
  std::weak_ptr<SimpleObject> weak;
  std::weak_ptr<CoreContext> subCtxt;

  {
    AutoCreateContext context;
    CurrentContextPusher psher(context);
    subCtxt = context;

    {
      // Okay, now we create a simple class first:
      AutoRequired<SimpleObject> sObj;
      ASSERT_TRUE(sObj.IsAutowired()) << "Failed to construct a simple type in the current context";

      weak = sObj;
    }

    // Verify that the object still exists (somewhere) even though we don't hold a reference directly
    // We expect the reference to be held open by our containing context
    ASSERT_FALSE(weak.expired()) << "An inserted object was deleted before the context was destroyed";
  }

  ASSERT_TRUE(subCtxt.expired()) << "A context survived beyond the point where it was expected to have become invalid";
  ASSERT_TRUE(weak.expired()) << "An object survived the destruction of its parent context";
}

TEST_F(ContextCleanupTest, VerifyContextDtor) {
  std::weak_ptr<CoreContext> contextVerifier;
  std::weak_ptr<SimpleObject> objVerifier;
  std::weak_ptr<CoreContext> subContextWeak;

  {
    // Create a new context and add some objects
    AutoCreateContext subContext;

    // Copy over the weak pointer before we go out of scope:
    contextVerifier = subContext;

    // Verify the use count is what we expect at this point, should be just the pointer itself:
    EXPECT_EQ(1, contextVerifier.use_count()) << "Unexpected reference count on CoreContext";

    {
      // Now make the context current, and check the new count
      CurrentContextPusher pshr(subContext);
      EXPECT_EQ(2, contextVerifier.use_count()) << "Context currency assignment altered use count unexpectedly";

      // Generate a new simple object:
      AutoRequired<SimpleObject> simple;
      objVerifier = simple;

      // Should be exactly five references to this object
      EXPECT_EQ(6, objVerifier.use_count()) << "Too many references to a newly constructed object";

      // Reference count should be unchanged:
      EXPECT_EQ(2, contextVerifier.use_count()) << "Reference count changed unexpectedly after addition of an object";

      // Eliminate the thread reference to this context:
      shared_ptr<CoreContext> ref = subContext;
      EXPECT_EQ(3, ref.use_count()) << "Pointer copy didn't increment the context reference count as expected";
      subContextWeak = subContext;
    }

    // Pop should decrement the reference count by two:  Once for the actual Autowired instance, and
    // again for the Autowired smart pointer itself.  This context will then be the only remaining
    // reference, and when it goes out of scope, the context will go away.
    EXPECT_EQ(1, subContextWeak.use_count()) << "Pop didn't decrement the context reference count as expected";
  }

  // The weak pointer to the context should be invalid by now:
  EXPECT_TRUE(contextVerifier.expired()) << "CoreContext still had " << contextVerifier.use_count() << " reference(s)";

  // The object should be gone, but will still be around if the context still exists:
  EXPECT_TRUE(objVerifier.expired()) << "SimpleObject still had " << objVerifier.use_count() << " reference(s)";
}

TEST_F(ContextCleanupTest, VerifyThreadCleanup) {
  // Create a context that will be used to test the cleanup:
  AutoCreateContext context;
  CurrentContextPusher pshr(context);

  // Add a simple thread object
  context->Add<SimpleThreaded>();

  // Kick off the operation
  context->InitiateCoreThreads();

  // No exit initially:
  EXPECT_FALSE(context->Wait(milliseconds(10))) << "Core context completed prematurely";

  Autowired<SimpleThreaded> simple;
  ASSERT_TRUE(simple) << "Couldn't autowire the SimpleThreaded object";

  // Context shutdown
  context->SignalShutdown();

  // Now we verify that exiting happens promptly:
  EXPECT_TRUE(context->Wait(milliseconds(100))) << "Context did not exit in a timely fashion";
}
