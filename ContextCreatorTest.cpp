#include "stdafx.h"
#include "ContextCreatorTest.h"
#include "ContextCreator.h"
#include <string>

extern const char gc_contextName[] = "eviction_context";

class Creator:
  public ContextCreator<gc_contextName, int> {
};

class GlobalSignal {
public:
  GlobalSignal(void):
    m_shouldContinue(false)
  {}

private:
  boost::mutex m_lock;
  bool m_shouldContinue;
  boost::condition_variable s_continueCond;
  
public:
  void Signal(void) {
    (boost::lock_guard<boost::mutex>)m_lock,
    (m_shouldContinue = true),
    s_continueCond.notify_all();
  }

  void Delay(void) {
    boost::unique_lock<boost::mutex> lk(m_lock);
    s_continueCond.wait(lk, [this] () {return m_shouldContinue;});
  }
};

class WaitMember:
  public CoreThread
{
public:
  WaitMember(void) {
    Ready();
  }

private:
  AutoRequired<GlobalSignal> m_signal;

public:

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
    ctxt = *creator->CreateContext(1);

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
  boost::mutex lock;
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
    for(size_t i = count; i--;) {
      std::shared_ptr<CoreContext> ctxt = *creator->CreateContext(i);
      CurrentContextPusher pshr(ctxt);

      // Trivial validation that the newly created context is an empty context:
      ASSERT_EQ(0, ctxt->GetMemberCount()) << "A created context was not empty";

      // Add in an object to test asynchronous destruction:
      AutoRequired<WaitMember> obj;
      members[i] = obj;

      // Add a notifier to signal a continue condition when we have everything we need:
      ctxt->AddTeardownListener([&lock, &cond, &counter] {
        (boost::lock_guard<boost::mutex>)lock,
        counter--,
        cond.notify_all();
      });

      // Kick off the context:
      ctxt->InitiateCoreThreads();
    }

    // Signal all members and then release everything:
    signal->Signal();
  }

  // Wait for all contexts to be destroyed
  boost::unique_lock<boost::mutex> lk(lock);
  cond.wait(lk, [&counter] {return counter == 0;});

  // Validate that everything expires:
  EXPECT_EQ(0, creator->GetSize()) << "Not all contexts were evicted as expected";
}