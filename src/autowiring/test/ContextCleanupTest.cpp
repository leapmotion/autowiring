// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/SimpleObject.hpp"
#include "TestFixtures/SimpleThreaded.hpp"
#include <autowiring/autowiring.h>
#include <autowiring/CoreContext.h>
#include THREAD_HEADER

class ContextCleanupTest:
  public testing::Test
{};

TEST_F(ContextCleanupTest, ValidateTeardownOrder) {
  class WeakPtrChecker {
  public:
    ~WeakPtrChecker(void) {
      EXPECT_TRUE(self.expired()) << "Shared pointer to this was not expired at destructor time";
    }

    std::weak_ptr<WeakPtrChecker> self;
  };
  
  // Construct, the destroy
  std::make_shared<WeakPtrChecker>();
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
    ASSERT_EQ(1, contextVerifier.use_count()) << "Unexpected reference count on CoreContext";

    {
      // Now make the context current, and check the new count
      CurrentContextPusher pshr(subContext);
      ASSERT_EQ(2, contextVerifier.use_count()) << "Context currency assignment altered use count unexpectedly";

      // Generate a new simple object:
      AutoRequired<SimpleObject> simple;
      objVerifier = simple;

      // Each CoreObjectDescriptor instance holds 2 strong references to SimpleObject, as CoreObject type and as ContextMember type.
      // One instance is held in CoreContext::m_concreteTypes and the other in the CoreContext::m_typeMemos.
      // Finally, once more reference is held by the shared_ptr<SimpleObject> inheritance of simple.
      ASSERT_EQ(5, objVerifier.use_count()) << "Unexpected number of references to a newly constructed object";

      // Reference count should be unchanged:
      ASSERT_EQ(2, contextVerifier.use_count()) << "Reference count changed unexpectedly after addition of an object";

      // Eliminate the thread reference to this context:
      auto ref = subContext;
      ASSERT_EQ(3, ref.use_count()) << "Pointer copy didn't increment the context reference count as expected";
      subContextWeak = subContext;
    }

    // Pop should decrement the reference count by two:  Once for the actual Autowired instance, and
    // again for the Autowired smart pointer itself.  This context will then be the only remaining
    // reference, and when it goes out of scope, the context will go away.
    ASSERT_EQ(1, subContextWeak.use_count()) << "Pop didn't decrement the context reference count as expected";
  }

  // The weak pointer to the context should be invalid by now:
  ASSERT_TRUE(contextVerifier.expired()) << "CoreContext still had " << contextVerifier.use_count() << " reference(s)";

  // The object should be gone, but will still be around if the context still exists:
  ASSERT_TRUE(objVerifier.expired()) << "SimpleObject still had " << objVerifier.use_count() << " reference(s)";
}

TEST_F(ContextCleanupTest, VerifyThreadCleanup) {
  // Create a context that will be used to test the cleanup:
  AutoCreateContext context;
  CurrentContextPusher pshr(context);

  // Add a simple thread object
  AutoRequired<SimpleThreaded>();

  // Kick off the operation
  context->Initiate();

  // No exit initially:
  ASSERT_FALSE(context->Wait(std::chrono::milliseconds(10))) << "Core context completed prematurely";

  Autowired<SimpleThreaded> simple;
  ASSERT_TRUE(simple) << "Couldn't autowire the SimpleThreaded object";

  // Context shutdown
  context->SignalShutdown();

  // Now we verify that exiting happens promptly:
  ASSERT_TRUE(context->Wait(std::chrono::milliseconds(100))) << "Context did not exit in a timely fashion";
}

class ReceivesTeardownNotice:
  public ContextMember
{
public:
  void NotifyContextTeardown(void) override {
    m_notified = true;
  }

  bool m_notified = false;
};

TEST_F(ContextCleanupTest, VerifyGracefulThreadCleanup) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();
  AutoRequired<CoreThread> ct;

  // Just create a CoreThread directly and have it pend some lambdas that will take awhile to run:
  auto called = std::make_shared<bool>(false);
  *ct += [] {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  };
  *ct += [called] { *called = true; };

  // Verify that a graceful shutdown ensures both lambdas are called:
  ASSERT_FALSE(ctxt->IsShutdown()) << "Context shut down prematurely";
  ctxt->SignalShutdown(true, ShutdownMode::Graceful);
  ASSERT_FALSE(ct->IsRunning()) << "Thread still reported as running even after a wait concluded";
  ASSERT_TRUE(*called) << "Graceful shutdown did not correctly run down all lambdas";
}

TEST_F(ContextCleanupTest, VerifyImmediateThreadCleanup) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();
  AutoRequired<CoreThread> ct;

  // Just create a CoreThread directly and have it pend some lambdas that will take awhile to run:
  auto called = std::make_shared<bool>(false);
  *ct += [] {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  };

  // Pend another lambda which will wait longer, for systems which will not complete SignalTerminate in 100ms
  *ct += [] {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  };

  // This lambda will return right away, and should still be present on the dispatch queue when SignalTerminate
  // returns.
  *ct += [called] { *called = true; };

  // Verify that a graceful shutdown ensures both lambdas are called:
  ctxt->SignalTerminate(true);
  ASSERT_FALSE(*called) << "Immediate shutdown incorrectly ran down the dispatch queue";
}

TEST_F(ContextCleanupTest, VerifyNotificationReciept) {
  std::shared_ptr<ReceivesTeardownNotice> rtn;
  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);

    // Now create an object which will receive a teardown notice:
    rtn = AutoRequired<ReceivesTeardownNotice>();
  }
  ASSERT_TRUE(rtn->m_notified) << "A member of a destroyed context did not correctly receive a teardown notice";
}

class TakesALongTimeToExit:
  public CoreThread
{
public:
  bool m_canContinue = false;

  void Continue(void) {
    PerformStatusUpdate([this] {m_canContinue = true; });
  }

  virtual void Run(void) {
    WaitForStateUpdate([this] { return m_canContinue; });
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
};

TEST_F(ContextCleanupTest, VerifyThreadShutdownInterleave) {
  // Record the initial use count:
  AutoCurrentContext ctxt;
  auto initCount = ctxt.use_count();

  // Create a thread that will take awhile to stop:
  AutoRequired<TakesALongTimeToExit> longTime;

  // We want threads to run as soon as they are added:
  ctxt->Initiate();

  // Make the thread exit before the enclosing context exits:
  longTime->Continue();
  longTime->Stop();

  // Now stop the context and perform an explicit wait
  ctxt->SignalShutdown(true);

  // At this point, the thread must have returned AND released its shared pointer to the enclosing context
  ASSERT_EQ(initCount, ctxt.use_count()) << "Context thread persisted even after it should have fallen out of scope";
}

