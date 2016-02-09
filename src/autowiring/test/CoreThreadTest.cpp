// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/SimpleThreaded.hpp"
#include <autowiring/at_exit.h>
#include <autowiring/Autowired.h>
#include <algorithm>
#include THREAD_HEADER

class CoreThreadTest:
  public testing::Test
{};

static_assert(
  std::is_same<
    decltype(std::chrono::steady_clock::now() + std::chrono::seconds(1)),
    std::chrono::steady_clock::time_point
  >::value,
  "Incrementing a steady clock by a second duration did not result in the expected interval type"
);

static_assert(
  std::is_same<
    decltype(std::chrono::steady_clock::now() + std::chrono::microseconds(1)),
    std::chrono::steady_clock::time_point
  >::value,
  "Incrementing a steady clock by a microsecond duration did not result in the expected interval type"
);

class SpamguardTest:
  public CoreThread
{
public:
  bool m_hit = false;
  bool m_multiHit = false;

  void Run(void) override {
    if(m_hit) {
      m_multiHit = true;
      return;
    }

    m_hit = false;

    CoreThread::Run();
  }
};

TEST_F(CoreThreadTest, VerifyStartSpam) {
  AutoCurrentContext ctxt;

  // Create our thread class:
  AutoRequired<SpamguardTest> instance;

  ctxt->Initiate();

  // This shouldn't cause another thread to be created:
  instance->Start(std::shared_ptr<CoreObject>((CoreObject*) 1, [] (CoreObject*) {}));

  EXPECT_FALSE(instance->m_multiHit) << "Thread was run more than once unexpectedly";
}

class InvokesIndefiniteWait:
  public CoreThread
{
public:
  virtual void Run(void) override {

    // Wait for one event using an indefinite timeout, then quit:
    WaitForEvent(std::chrono::steady_clock::time_point::max());
  }
};

TEST_F(CoreThreadTest, VerifyIndefiniteDelay) {
  AutoCurrentContext ctxt;

  AutoRequired<InvokesIndefiniteWait> instance;
  ctxt->Initiate();

  // Verify that the instance does not quit until we pend something:
  ASSERT_FALSE(instance->WaitFor(std::chrono::milliseconds(10))) << "Thread instance exited prematurely, when it should have been waiting indefinitely";

  // Now we pend an arbitrary event and verify that we can wait:
  *instance += [] {};
  ASSERT_TRUE(instance->WaitFor(std::chrono::milliseconds(10))) << "Instance did not exit after an event was pended which should have woken up its dispatch loop";
}

TEST_F(CoreThreadTest, VerifyNestedTermination) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  std::shared_ptr<SimpleThreaded> st;

  // Insert a thread into a second-order subcontext:
  {
    AutoCreateContext outer;
    CurrentContextPusher outerPshr(outer);
    AutoRequired<SimpleThreaded>();
    outer->Initiate();

    {
      AutoCreateContext ctxt;
      CurrentContextPusher pshr(ctxt);
      ctxt->Initiate();
      st = AutoRequired<SimpleThreaded>();
    }
  }

  // Everything should be running by now:
  ASSERT_TRUE(st->IsRunning()) << "Child thread was not running as expected";

  // Shut down the enclosing context:
  ctxt->SignalTerminate(true);

  // Verify that the child thread has stopped:
  ASSERT_FALSE(st->IsRunning()) << "Child thread was running even though the enclosing context was terminated";
}

class SleepEvent {
public:
  virtual void SleepFor(int seconds) = 0;
};

class ListenThread :
  public CoreThread,
  public SleepEvent
{
public:
  ListenThread() : CoreThread("ListenThread") {}

  void SleepFor(int seconds) override {
    *this += [this] {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      if(ShouldStop())
        throw std::runtime_error("Execution aborted");
    };
  }
};

TEST_F(CoreThreadTest, AUTOTHROW_VerifyDispatchQueueShutdown) {
  AutoCreateContext ctxt;
  CurrentContextPusher pusher(ctxt);

  AutoRequired<ListenThread> listener;
  try
  {
    ctxt->Initiate();

    AutoFired<SleepEvent> evt;

    // Spam in a bunch of events:
    for(size_t i = 100; i--;)
      evt(&SleepEvent::SleepFor)(0);

    // Graceful termination then enclosing context shutdown:
    listener->Stop(true);
    ctxt->SignalShutdown(true);
  }
  catch (...) {}

}

TEST_F(CoreThreadTest, AUTOTHROW_VerifyNoLeakOnExecptions) {
  AutoCurrentContext ctxt;

  AutoRequired<ListenThread> listener;
  auto value = std::make_shared<std::string>("sentinal");

  std::weak_ptr<std::string> watcher(value);
  try
  {
    ctxt->Initiate();

    *listener += [value] { throw std::exception(); };
    value.reset();
    ctxt->SignalShutdown(true);
  }
  catch (...) {}

  ASSERT_TRUE(watcher.expired()) << "Leaked memory on exception in a dispatch event";
}

TEST_F(CoreThreadTest, VerifyDelayedDispatchQueueSimple) {
  AutoCurrentContext ctxt;

  // Run our threads immediately, no need to wait
  ctxt->Initiate();

  // Create a thread which we'll use just to pend dispatch events:
  AutoRequired<CoreThread> t;

  // Thread should be running by now:
  ASSERT_TRUE(t->IsRunning()) << "Thread added to a running context was not marked running";

  // Delay until the dispatch loop is actually running, then wait an additional 1ms to let the
  // WaitForEvent call catch on:
  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  // These are flags--we'll set them to true as the test proceeds
  auto x = std::make_shared<bool>(false);
  auto y = std::make_shared<bool>(false);

  // Pend a delayed event first, and then an immediate event right afterwards:
  *t += std::chrono::hours(1), [x] { *x = true; };
  *t += [y] { *y = true; };

  // Spin down until the number of ready dispatchers is one (the single delayed dispatcher):
  while (t->GetDispatchQueueLength() > 1)
    std::this_thread::yield();
  ASSERT_TRUE(*y) << "A simple ready call was not dispatched within 100ms of being pended";
  ASSERT_FALSE(*x) << "An event which should not have been executed for an hour was executed early";
}

TEST_F(CoreThreadTest, VerifyNoDelayDoubleFree) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  // We won't actually be referencing this, we just want to make sure it's not destroyed early
  auto x = std::make_shared<bool>();

  // This deferred pend will never actually be executed:
  AutoRequired<CoreThread> t;
  *t += std::chrono::hours(1), [x] {};

  // Verify that we have exactly one pended event at this point.
  ASSERT_EQ(1UL, t->GetDispatchQueueLength()) << "Dispatch queue had an unexpected number of pended events";

  // Verify that the shared pointer isn't unique at this point.  If it is, it's because our CoreThread deleted
  // the event even though it was supposed to have pended it.
  ASSERT_FALSE(x.unique()) << "A pended event was freed before it was called, and appears to be present in a dispatch queue";
}

TEST_F(CoreThreadTest, VerifyDoublePendedDispatchDelay) {
  AutoCurrentContext ctxt;

  // Immediately pend threads:
  ctxt->Initiate();

  // Some variables that we will set to true as the test proceeds:
  auto x = std::make_shared<bool>(false);
  auto y = std::make_shared<bool>(false);

  // Create a thread as before, and pend a few events.  The order, here, is important.  We intentionally
  // pend an event that won't happen for awhile, in order to trick the dispatch queue into waiting for
  // a lot longer than it should for the next event.
  AutoRequired<CoreThread> t;
  *t += std::chrono::hours(1), [x] { *x = true; };

  // Now pend an event that will be ready just about right away:
  *t += std::chrono::microseconds(1), [y] { *y = true; };

  // Delay for a short period of time, then check our variable states:
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  // This one shouldn't have been hit yet, it isn't scheduled to be hit for 1hr
  ASSERT_FALSE(*x) << "A delayed dispatch was invoked extremely early";

  // This one should become ready very fast, but we won't wait long for it:
  auto start = std::chrono::high_resolution_clock::now();
  while(std::chrono::high_resolution_clock::now() - start < std::chrono::seconds(1))
    if(*y)
      return;

  FAIL() << "An out-of-order delayed dispatch was not executed in time as expected";
}

TEST_F(CoreThreadTest, VerifyTimedSort) {
  AutoCurrentContext ctxt;
  AutoRequired<CoreThread> t;

  std::vector<size_t> v;

  // Pend a stack of lambdas.  Each lambda waits 3i milliseconds, and pushes the value
  // i to the back of a vector.  If the delay method is implemented correctly, the resulting
  // vector will always wind up sorted, no matter how we push elements to the queue.
  // To doubly verify this property, we don't trivially increment i from the minimum to the
  // maximum--rather, we use a simple PRNG called a linear congruential generator and hop around
  // the interval [1...12] instead.
  auto base = std::chrono::steady_clock::now();
  for(size_t i = 1; i != 0; i = (i * 5 + 1) % 16)
    *t += (base + std::chrono::microseconds(i * 3)), [&v, i] { v.push_back(i); };

  // Delay for the thread to finish up.  Technically this is 11ms more than we need.
  ctxt->Initiate();
  while(t->GetDispatchQueueLength())
    std::this_thread::yield();

  // Verify that the resulting vector is sorted.
  ASSERT_TRUE(std::is_sorted(v.begin(), v.end())) << "A timed sort implementation did not generate a sorted sequence as expected";
}

TEST_F(CoreThreadTest, VerifyPendByTimePoint) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();
  AutoRequired<CoreThread> t;

  // Pend by an absolute time point, nothing really special here
  auto x = std::make_shared<bool>(false);
  *t += (std::chrono::steady_clock::now() + std::chrono::milliseconds(1)), [&t, x] {
    *x = true;
    t->Stop();
  };

  // Verify that this lambda does not run right away:
  ASSERT_FALSE(*x) << "A timepoint-based delayed dispatch was invoked early";

  // Verify that we hit this after one ms of delay
  ASSERT_TRUE(t->WaitFor(std::chrono::seconds(5))) << "A timepoint-based delayed dispatch was not invoked in a timely fashion";
}

TEST_F(CoreThreadTest, WaitBeforeInitiate) {
  AutoCurrentContext ctxt;

  // Wait for the outer context before it's actually initiated:
  ASSERT_FALSE(ctxt->Wait(std::chrono::seconds(0))) << "A wait operation on a context succeeded even though it was not yet started";

  // Stop the outer context without even starting it
  ctxt->SignalTerminate();

  // Try to add a thread to the context.  This should cause the thread to be immediately marked as "stopped" and should allow a Wait
  // operation on the thread to return right away.
  AutoRequired<CoreThread> ct;
  ASSERT_TRUE(!ct->IsRunning()) << "A thread added to an already-stopped context was incorrectly marked as running";
  ASSERT_TRUE(ct->ShouldStop()) << "A thread added to an already-stopped context did not report that it should be stopped";
  ASSERT_TRUE(ct->WaitFor(std::chrono::seconds(0))) << "A thread added to an already-stopped context did not correctly respond to a zero-duration wait";

  // Wait again.  This should suceed right away.
  ASSERT_TRUE(ctxt->Wait(std::chrono::seconds(0))) << "Failed to wait on a context which should have already been stopped";
}

TEST_F(CoreThreadTest, ReentrantStopOnTeardown) {
  AutoCurrentContext ctxt;
  std::shared_ptr<CoreThread> ct = AutoRequired<CoreThread>();

  // Our thread will try to stop itself on teardown, this will cause it to trivially deadlock if any locks
  // are held while it passes notification to listeners:
  ct->AddTeardownListener([ct] {
    ct->Stop();
  });

  // Kickoff and then delay:
  ctxt->Initiate();
  ctxt->SignalShutdown();
  ASSERT_TRUE(ctxt->Wait(std::chrono::seconds(1))) << "Thread deadlocked when attempting to stop itself from a teardown listener";
}

class VerifiesCurrentContextOnRundown:
  public CoreThread
{
public:
  VerifiesCurrentContextOnRundown(void):
    CoreThread("VerifiesCurrentContextOnRundown")
  {
    *this += [] { AutoCurrentContext()->SignalShutdown(); };
    *this += [this] { m_ctxt = AutoCurrentContext(); };
  }

  std::shared_ptr<CoreContext> m_ctxt;
};

TEST_F(CoreThreadTest, EnsureContextCurrencyInRundown) {
  AutoCurrentContext ctxt;

  // Create our tester class, and then shut down the context.  This should cause a full rundown
  // on the dispatch queue.
  AutoRequired<VerifiesCurrentContextOnRundown> tester;

  // Need to be absolutely sure that the cyclic reference the tester creates is cleaned up:
  auto clean = MakeAtExit([&tester] { tester->m_ctxt.reset(); });

  ctxt->Initiate();
  ASSERT_TRUE(ctxt->Wait(std::chrono::seconds(5))) << "Context did not shut down in a timely fashion";
  ASSERT_EQ(ctxt, tester->m_ctxt) << "Current context was not preserved while a CoreThread was being run down";
}

TEST_F(CoreThreadTest, AbandonedDispatchers) {
  auto v = std::make_shared<bool>(false);
  AutoRequired<CoreThread> ct;
  *ct += [v] { *v = true; };

  // Graceful shutdown on our enclosing context without starting it:
  AutoCurrentContext()->SignalShutdown(true);

  // Verify that all lambdas on the CoreThread got called as expected:
  ASSERT_FALSE(*v) << "Lambdas attached to a CoreThread should not be executed when the enclosing context is terminated without being started";
}

TEST_F(CoreThreadTest, PendAfterShutdown) {
  auto v = std::make_shared<bool>(false);

  // Create a CoreThread and pend a lambda to it after terminating the enclosing context.  This will let us track
  // what happens to the destructor operations on the lambda.
  AutoCurrentContext()->SignalShutdown(true);

  // We should still be able to inject into a terminated context--the thread just won't start
  AutoRequired<CoreThread> ct;
  *ct += [v] {};

  // The dispatch queue length can't have increased as a result of the above.  Again, the CoreThread is supposed
  // to be terminated by this point and not even running; any dispatchers pended at this point are going to get
  // leaked until the enclosing context is destroyed.  It would be a different story if the CoreContext were in
  // rundown mode right now and the thread is still running but wrapping up, but that's definitely not the case
  // here.
  ASSERT_EQ(0UL, ct->GetDispatchQueueLength()) << "A dispatcher was incorrectly pended to a CoreThread that was not running";

  // Verify that the lambda was destroyed more or less right away
  ASSERT_TRUE(v.unique()) << "Shared pointer in a lambda closure appears to have been leaked";
}

class BTOverridesOnStopHandler:
  public BasicThread
{
public:
  bool got_stopped = false;

  void Run(void) override {}

  void OnStop(void) override { got_stopped = true; }
};

class CTOverridesOnStopHandler:
  public CoreThread
{
public:
  bool got_stopped = false;
  void OnStop(void) override { got_stopped = true; }
};

TEST_F(CoreThreadTest, VerifyThreadGetsOnStop) {
  AutoRequired<BTOverridesOnStopHandler> bsoosh;
  AutoRequired<CTOverridesOnStopHandler> ctoosh;

  AutoCurrentContext ctxt;
  ctxt->Initiate();
  ctxt->SignalShutdown();
  ctxt->Wait();

  EXPECT_TRUE(bsoosh->got_stopped) << "BasicThread instance did not receive an OnStop notification as expected";
  EXPECT_TRUE(ctoosh->got_stopped) << "CoreThread instance did not receive an OnStop notification as expected";
}

class MakesPassiveCallInOnStop:
  public CoreThread
{
public:
  // Run operation is left empty, we want our thread to stop right after it starts
  void Run(void) override {}

  bool bStartExcepted = false;
  bool bStopExcepted = false;

  bool OnStart(void) override {
    try {
      std::lock_guard<std::mutex> lk(GetLock());
    }
    catch (...) {
      bStartExcepted = true;
    }
    return CoreThread::OnStart();
  }

  void OnStop(void) override {
    try {
      // Causes an already-obtained exception if we are in rundown
      std::lock_guard<std::mutex> lk(GetLock());
    }
    catch (...) {
      bStopExcepted = true;
    }
  }
};

TEST_F(CoreThreadTest, LightsOutPassiveCall) {
  AutoCurrentContext ctxt;
  AutoRequired<MakesPassiveCallInOnStop> mpc;
  ctxt->Initiate();

  // Wait for bad things to happen
  ASSERT_TRUE(mpc->WaitFor(std::chrono::seconds(5))) << "Passive call thread took too long to quit";

  // Verify no bad things happened:
  ASSERT_FALSE(mpc->bStartExcepted) << "Exception occurred during an attempt to elevate to synchronized level from CoreThread::OnStart";
  ASSERT_FALSE(mpc->bStopExcepted) << "Exception occurred during an attempt to elevate to synchronized level from CoreThread::OnStop";
}

class CoreThreadExtraction:
  public CoreThread
{
public:
  using CoreThread::m_queueUpdated;
};

TEST_F(CoreThreadTest, SpuriousWakeupTest) {
  AutoCurrentContext()->Initiate();
  AutoRequired<CoreThreadExtraction> extraction;

  std::mutex lock;
  std::condition_variable cv;
  bool ready = false;
  size_t countOnWake = -1;

  auto wakeFn = [&] {
    std::lock_guard<std::mutex>{ lock },
    ready = true;
    cv.notify_all();
    countOnWake = extraction->GetDispatchQueueLength();
  };

  // Add a lambda that should launch right away and a delayed lambda that we know won't launch
  *extraction += wakeFn;
  *extraction += std::chrono::hours(1), [] {};

  // Delay until the extraction lambda has run:
  std::unique_lock<std::mutex> lk(lock);
  ASSERT_TRUE(cv.wait_for(lk, std::chrono::seconds(5), [&] { return ready; }));

  // Now force a spurious wakeup--this shouldn't technically be a problem
  extraction->m_queueUpdated.notify_all();

  // Delayed wake function, block for this to happen:
  ready = false;
  *extraction += std::chrono::milliseconds(1), wakeFn;
  ASSERT_TRUE(cv.wait_for(lk, std::chrono::seconds(5), [&] { return ready; }));

  ASSERT_EQ(2UL, countOnWake) << "Dispatch queue changed size under a spurious wakeup condition";
}

class BlocksInOnStop:
  public CoreThread
{
public:
  bool is_waiting = false;
  bool signal = false;

  void Run(void) override {
    // Let the run loop return.  This triggers cleanup operations and ultimately causes OnStop
    // to get called in our own thread context.
  }

  bool Block(std::chrono::nanoseconds timeout) {
    std::unique_lock<std::mutex> lk(m_lock);
    return m_cv.wait_for(lk, timeout, [this] {return is_waiting; });
  }

  void Continue(void) {
    std::lock_guard<std::mutex>{ m_lock },
    signal = true;
    m_cv.notify_all();
  }

  void OnStop(void) override {
    std::unique_lock<std::mutex> lk(this->m_lock);
    is_waiting = true;
    m_cv.notify_all();
    m_cv.wait_for(lk, std::chrono::seconds(5), [this] { return signal; });
  }
};

TEST_F(CoreThreadTest, ContextWaitTimesOutInOnStop) {
  AutoCurrentContext ctxt;
  AutoRequired<BlocksInOnStop> bios;
  ctxt->Initiate();

  // Wait for our pathological case to be waiting before we try shutting down the context
  ASSERT_TRUE(bios->Block(std::chrono::seconds(5))) << "Blocking class failed to enter a blocked condition as expected";

  ctxt->SignalShutdown();
  ASSERT_FALSE(bios->WaitFor(std::chrono::milliseconds(1))) << "Timed wait on a misbehaving CoreThread did not time out as expected";
  ASSERT_FALSE(ctxt->Wait(std::chrono::milliseconds(1))) << "Context wait routine did not timeout as expected";

  // Let BIOS back out now:
  bios->Continue();
  ASSERT_TRUE(ctxt->Wait(std::chrono::seconds(5))) << "Context did not complete in a timely fashion";
  ASSERT_EQ(2UL, ctxt.use_count()) << "Entity held a context shared pointer after teardown has taken place";
}

TEST_F(CoreThreadTest, SubContextHoldsParentContext) {
  AutoCurrentContext parent;
  size_t initUses = parent.use_count();
  {
    AutoCreateContext child;
    auto thread = child->Inject<CoreThread>();

    auto cv = std::make_shared<std::condition_variable>();
    auto lock = std::make_shared<std::mutex>();
    auto proceed = std::make_shared<bool>(false);
    *thread += [cv, lock, proceed] {
      std::unique_lock<std::mutex> lk(*lock);
      cv->wait_for(lk, std::chrono::seconds(5), [&] { return *proceed; });
    };

    parent->Initiate();
    child->Initiate();

    // Terminate the parent, verify that the child has exited:
    parent->SignalShutdown();
    ASSERT_FALSE(parent->Wait(std::chrono::milliseconds(5))) << "Parent context returned before all child threads were done";
    std::lock_guard<std::mutex> { *lock },
    *proceed = true;
    cv->notify_all();
    ASSERT_TRUE(parent->Wait(std::chrono::seconds(5))) << "Signalled thread did not terminate in a timely fashion";
  }
  ASSERT_EQ(initUses, parent.use_count()) << "Parent had spurious references after all objects should have been destroyed";
}

class DoesNothingButQuit:
  public CoreThread
{
public:
  void Run(void) override {}
};

TEST_F(CoreThreadTest, QuiescentNotTerminated) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();
  AutoRequired<DoesNothingButQuit> ct;
  ASSERT_TRUE(ctxt->Quiescent(std::chrono::seconds(5))) << "Quiescence was not achieved in a thread that should have self-terminated";
  ASSERT_TRUE(ctxt->IsQuiescent()) << "Delayed for quiescence, but this was not the state entered when checked on return";
}

TEST_F(CoreThreadTest, LambdaHoldAfterTermination) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  std::weak_ptr<CoreContext> childWeak;
  {
    AutoCreateContext child;
    AutoRequired<CoreThread> mct{ child };

    // This is the critical piece which generates a hold on `child` after teardown
    mct->AddTeardownListener([mct, child] {
      *mct += [child] {};
    });

    child->Initiate();
    *mct += [] {};
    mct->Barrier();
    child->SignalShutdown();
    childWeak = child;
    child->Wait();
  }
  ASSERT_TRUE(childWeak.expired()) << "Child context leaked due to lambda pending in teardown";
}
