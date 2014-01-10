// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "CoreThreadTest.h"
#include "Autowired.h"
#include "TestFixtures/SimpleThreaded.h"

class SpamguardTest:
  public CoreThread
{
public:
  SpamguardTest(void):
    m_hit(false),
    m_multiHit(false)
  {
  }

  bool m_hit;
  bool m_multiHit;

  void Run(void) override {
    if(m_hit) {
      m_multiHit = true;
      return;
    }

    m_hit = false;

    boost::unique_lock<boost::mutex> lk(m_lock);
    m_stateCondition.wait(lk, [this] () {return ShouldStop();});
  }
};

TEST_F(CoreThreadTest, VerifyStartSpam) {
  // Create our thread class:
  AutoRequired<SpamguardTest> instance;

  m_create->InitiateCoreThreads();

  // This shouldn't cause another thread to be created:
  instance->Start(std::shared_ptr<Object>(new Object));

  EXPECT_FALSE(instance->m_multiHit) << "Thread was run more than once unexpectedly";
}

class InvokesIndefiniteWait:
  public CoreThread
{
public:
  virtual void Run(void) override {
    AcceptDispatchDelivery();

    // Wait for one event using an indefinite timeout, then quit:
    WaitForEvent(boost::chrono::steady_clock::time_point::max());
  }
};

TEST_F(CoreThreadTest, VerifyIndefiniteDelay) {
  AutoRequired<InvokesIndefiniteWait> instance;
  m_create->InitiateCoreThreads();

  // Verify that the instance does not quit until we pend something:
  ASSERT_FALSE(instance->WaitFor(boost::chrono::milliseconds(10))) << "Thread instance exited prematurely, when it should have been waiting indefinitely";

  // Now we pend an arbitrary event and verify that we can wait:
  *instance += [] {};
  ASSERT_TRUE(instance->WaitFor(boost::chrono::milliseconds(10))) << "Instance did not exit after an event was pended which should have woken up its dispatch loop";
}

TEST_F(CoreThreadTest, VerifyNestedTermination) {
  std::shared_ptr<SimpleThreaded> st;

  // Insert a thread into a second-order subcontext:
  {
    AutoCreateContext outer;
    CurrentContextPusher outerPshr(outer);
    AutoRequired<SimpleThreaded>();
    outer->InitiateCoreThreads();

    {
      AutoCreateContext ctxt;
      CurrentContextPusher pshr(ctxt);
      ctxt->InitiateCoreThreads();
      st = AutoRequired<SimpleThreaded>();
    }
  }

  // Everything should be running by now:
  ASSERT_TRUE(st->IsRunning()) << "Child thread was not running as expected";

  // Shut down the enclosing context:
  m_create->SignalTerminate(true);

  // Verify that the child thread has stopped:
  ASSERT_FALSE(st->IsRunning()) << "Child thread was running even though the enclosing context was terminated";
}