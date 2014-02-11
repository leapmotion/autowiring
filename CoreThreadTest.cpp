// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "CoreThreadTest.h"
#include "Autowired.h"
#include "TestFixtures/SimpleThreaded.h"
#include <thread>

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

class SleepEvent : public virtual EventReceiver
{
public:
  virtual Deferred SleepFor(int seconds) = 0;
  virtual Deferred SleepForThenThrow(int seconds) = 0;
};

class DispatchThread :
  public CoreThread
{
public:
  DispatchThread() : CoreThread("DispatchThread") {}
  virtual void Run() override{
    AutoFired<SleepEvent> evt;
    while (!ShouldStop())
    {
      std::cout << "loop" << std::endl;
      evt(&SleepEvent::SleepFor)(5);
    }


    ASSERT_EQ(GetDispatchQueueLength(), 0);
  }

};

class ListenThread :
  public CoreThread,
  public SleepEvent
{
public:
  ListenThread() : CoreThread("ListenThread") {}

  Deferred SleepFor(int seconds) override {
    unsigned int i = 0;
    while (i < 1000000 * (unsigned int)seconds){
      i++;
    }
    return Deferred();
  }

  Deferred SleepForThenThrow(int seconds) override {
    unsigned int i = 0;
    while (i < 1000000 * (unsigned int)seconds){
      i++;
    }
    throw std::exception();
    return Deferred();
  }

  virtual void Run() override{
    AcceptDispatchDelivery();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    CoreThread::Run();
    ASSERT_EQ(GetDispatchQueueLength(), 0);
  }

};

TEST_F(CoreThreadTest, VerifyDispatchQueueShutdown) {
  AutoCreateContext ctxt;
  CurrentContextPusher pusher(ctxt);

  AutoRequired<DispatchThread> dispatcher;
  AutoRequired<ListenThread> listener;
  try
  {
    ctxt->InitiateCoreThreads();

    std::this_thread::sleep_for(std::chrono::seconds(1));
    AutoFired<SleepEvent> evt;
    evt(&SleepEvent::SleepForThenThrow)(0);
    ctxt->SignalShutdown(true);
  }
  catch (...)
  {
    auto ptr = std::current_exception();
  }
  ASSERT_EQ(dispatcher->GetDispatchQueueLength(), 0);
  ASSERT_EQ(listener->GetDispatchQueueLength(), 0);
}