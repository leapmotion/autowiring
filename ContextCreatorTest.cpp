#include "stdafx.h"
#include "ContextCreatorTest.h"
#include "ContextCreator.h"
#include "CoreContext.h"
#include "CoreThread.h"
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <string>

struct EvictionContext {};

class Creator:
  public ContextCreator<EvictionContext, int>
{
};

class VoidCreator:
  public ContextCreator<EvictionContext>
{
public:
  VoidCreator(void):
    m_totalDestroyed(0)
  {}

  size_t m_totalDestroyed;

  void NotifyContextDestroyed(t_callbackHandle q, CoreContext* pContext) override {
    m_totalDestroyed++;
    ContextCreator<EvictionContext>::NotifyContextDestroyed(q, pContext);
  }
};

class GlobalSignal {
public:
  GlobalSignal(void):
    m_shouldContinue(false)
  {}

private:
  std::mutex m_lock;
  bool m_shouldContinue;
  std::condition_variable s_continueCond;

public:
  void Signal(void) {
    (std::lock_guard<std::mutex>)m_lock,
    (m_shouldContinue = true),
    s_continueCond.notify_all();
  }

  void Delay(void) {
    std::unique_lock<std::mutex> lk(m_lock);
    s_continueCond.wait(lk, [this] () {return m_shouldContinue;});
  }
};

class WaitMember:
  public CoreThread
{
public:
  Autowired<GlobalSignal> m_signal;

  void Run(void) override {
    // Wait for our event to be signalled, then leave
    m_signal->Delay();
  }
};

TEST_F(ContextCreatorTest, ValidateSimpleEviction) {
  // Create a context and verify it gets evicted from the context creator:
  AutoRequired<Creator> creator;
  std::weak_ptr<CoreContext> ctxtWeak;

  {
    std::shared_ptr<CoreContext> ctxt;

    // Make a context:
    ctxt = creator->CreateContext(1).first;

    // Obtain a weak pointer, in order to ensure proper teardown:
    ctxtWeak = ctxt;
  }

  // Context must be destroyed as a precondition of the subsequent assertion
  ASSERT_TRUE(ctxtWeak.expired()) << "Expected the context to be destroyed";

  // Verify that our creator is now empty:
  EXPECT_EQ(0UL, creator->GetSize()) << "Context creator is non-empty after all created contexts were destroyed";
}

TEST_F(ContextCreatorTest, ValidateMultipleEviction) {
  // Number of dependent contexts to be created
  const size_t count = 100;

  // Teardown lock, counter, and condition:
  std::mutex lock;
  boost::condition cond;
  int counter = count;

  // Obtain creator pointer:
  AutoRequired<Creator> creator;

  // Set up a signal manager at global context scope:
  AutoRequired<GlobalSignal> signal;

  {
    // Array of objects to test destruction on, and corresponding collection of contexts:
    std::shared_ptr<WaitMember> members[count];

    // Create a few contexts:
    for(int i = count; i--;) {
      AutoCreateContext ctxt;
      CurrentContextPusher pshr(ctxt);

      // Trivial validation that the newly created context is an empty context:
      ASSERT_EQ(static_cast<size_t>(0), ctxt->GetMemberCount()) << "A created context was not empty";

      // Add in an object to test asynchronous destruction:
      AutoRequired<WaitMember> obj;
      members[i] = obj;
      ASSERT_EQ(signal, obj->m_signal) << "Dependent context wiring did not correctly match to the enclosing scope";

      // Add a notifier to signal a continue condition when we have everything we need:
      ctxt->AddTeardownListener([&lock, &cond, &counter] {
        (std::lock_guard<std::mutex>)lock,
        counter--,
        cond.notify_all();
      });

      // Kick off the context:
      ctxt->Initiate();
    }

    // Signal all members and then release everything:
    signal->Signal();
  }

  // Wait for all contexts to be destroyed
  std::unique_lock<std::mutex> lk(lock);
  cond.wait(lk, [&counter] {return counter == 0;});

  // Validate that everything expires:
  EXPECT_EQ(static_cast<size_t>(0), creator->GetSize()) << "Not all contexts were evicted as expected";
}

TEST_F(ContextCreatorTest, ClearAndTeardown) {
  // Create a context and verify it gets evicted from the context creator:

  std::weak_ptr<CoreContext> ctxtWeak;

  {
    AutoCreateContext mainContext;
    CurrentContextPusher pusher(mainContext);

    AutoRequired<Creator> creator;
    std::shared_ptr<CoreContext> ctxt;

    // Make a sub-context
    ctxt = creator->CreateContext(0).first;

    // Obtain a weak pointer, in order to ensure proper teardown:
    ctxtWeak = ctxt;

    //Call clear on the creator.
    creator->Clear(true);

    //Make another one!
    ctxt = creator->CreateContext(1).first;
    //Let the creator go out of scope
  }

  // Context must be destroyed as a precondition of the subsequent assertion
  ASSERT_TRUE(ctxtWeak.expired()) << "Expected the context to be destroyed";

  // Verify that our creator is now empty:
 // EXPECT_EQ(0UL, creator->GetSize()) << "Context creator is non-empty after all created contexts were destroyed";
}

TEST_F(ContextCreatorTest, VoidKeyType) {
  AutoRequired<VoidCreator> vc;

  {
    std::shared_ptr<CoreContext> ctxt = vc->CreateContext();
    EXPECT_EQ(1UL, vc->GetSize()) << "Requested that a context be created, but the void creator did not have any members";

    EXPECT_EQ(1UL, vc->GetSize()) << "A created context was apparently destroyed after firing bolts";
    EXPECT_EQ(0UL, vc->m_totalDestroyed) << "The void creator received a NotifyContextDestroyed call unexpectedly early";

    vc->Clear(true);

    //Make another one to check about collisions
    std::shared_ptr<CoreContext> ctxt2 = vc->CreateContext();
    EXPECT_EQ(1UL, vc->GetSize()) << "Second void context creation failed!";
  }

  EXPECT_EQ(0UL, vc->GetSize()) << "A void context creator was not correctly updated when its dependent context went out of scope";
  EXPECT_EQ(2UL, vc->m_totalDestroyed) << "The void creator did not receive the expected number of NotifyContextDestroyed calls";
}
