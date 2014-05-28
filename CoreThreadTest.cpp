// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "CoreThreadTest.h"
#include "Autowired.h"
#include "at_exit.h"
#include "TestFixtures/SimpleThreaded.h"
#include <boost/thread/thread.hpp>
#include <boost/thread.hpp>

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

    CoreThread::Run();
  }
};

TEST_F(CoreThreadTest, VerifyStartSpam) {
  // Create our thread class:
  AutoRequired<SpamguardTest> instance;

  m_create->Initiate();

  // This shouldn't cause another thread to be created:
  instance->Start(std::shared_ptr<Object>((Object*) 1, [] (Object*) {}));

  EXPECT_FALSE(instance->m_multiHit) << "Thread was run more than once unexpectedly";
}

class InvokesIndefiniteWait:
  public CoreThread
{
public:
  virtual void Run(void) override {

    // Wait for one event using an indefinite timeout, then quit:
    WaitForEvent(boost::chrono::steady_clock::time_point::max());
  }
};

TEST_F(CoreThreadTest, VerifyIndefiniteDelay) {
  AutoRequired<InvokesIndefiniteWait> instance;
  m_create->Initiate();

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

class ListenThread :
  public CoreThread,
  public SleepEvent
{
public:
  ListenThread() : CoreThread("ListenThread") {}

  Deferred SleepFor(int seconds) override {
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
    if(ShouldStop())
      throw std::runtime_error("Execution aborted");

    return Deferred(this);
  }

  Deferred SleepForThenThrow(int seconds) override {
    return Deferred(this);
  }
};

TEST_F(CoreThreadTest, VerifyDispatchQueueShutdown) {
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

TEST_F(CoreThreadTest, VerifyNoLeakOnExecptions) {
  AutoCreateContext ctxt;
  CurrentContextPusher pshr(ctxt);

  AutoRequired<ListenThread> listener;
  std::shared_ptr<std::string> value(new std::string("sentinal"));

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
  // Run our threads immediately, no need to wait
  m_create->Initiate();

  // Create a thread which we'll use just to pend dispatch events:
  AutoRequired<CoreThread> t;

  // Thread should be running by now:
  ASSERT_TRUE(t->IsRunning()) << "Thread added to a running context was not marked running";

  // Delay until the dispatch loop is actually running, then wait an additional 1ms to let the
  // WaitForEvent call catch on:
  boost::this_thread::sleep_for(boost::chrono::milliseconds(1));

  // These are flags--we'll set them to true as the test proceeds
  std::shared_ptr<bool> x(new bool(false));
  std::shared_ptr<bool> y(new bool(false));

  // Pend a delayed event first, and then an immediate event right afterwards:
  *t += boost::chrono::hours(1), [x] { *x = true; };
  *t += [y] { *y = true; };

  // Verify that, after 100ms, the first event is called and the second event is NOT called:
  boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
  ASSERT_TRUE(*y) << "A simple ready call was not dispatched within 100ms of being pended";
  ASSERT_FALSE(*x) << "An event which should not have been executed for an hour was executed early";
}

TEST_F(CoreThreadTest, VerifyNoDelayDoubleFree) {
  m_create->Initiate();

  // We won't actually be referencing this, we just want to make sure it's not destroyed early
  std::shared_ptr<bool> x(new bool);

  // This deferred pend will never actually be executed:
  AutoRequired<CoreThread> t;
  *t += boost::chrono::hours(1), [x] {};

  // Verify that we have exactly one pended event at this point.
  ASSERT_EQ(1UL, t->GetDispatchQueueLength()) << "Dispatch queue had an unexpected number of pended events";

  // Verify that the shared pointer isn't unique at this point.  If it is, it's because our CoreThread deleted
  // the event even though it was supposed to have pended it.
  ASSERT_FALSE(x.unique()) << "A pended event was freed before it was called, and appears to be present in a dispatch queue";
}

TEST_F(CoreThreadTest, DISABLED_VerifyDoublePendedDispatchDelay) {
  // Immediately pend threads:
  m_create->Initiate();

  // Some variables that we will set to true as the test proceeds:
  std::shared_ptr<bool> x(new bool(false));
  std::shared_ptr<bool> y(new bool(false));

  // Create a thread as before, and pend a few events.  The order, here, is important.  We intentionally
  // pend an event that won't happen for awhile, in order to trick the dispatch queue into waiting for
  // a lot longer than it should for the next event.
  AutoRequired<CoreThread> t;
  *t += boost::chrono::hours(1), [x] { *x = true; };

  // Now pend an event that will be ready just about right away:
  *t += boost::chrono::nanoseconds(1), [y] { *y = true; };

  // Delay for a short period of time, then check our variable states:
  boost::this_thread::sleep_for(boost::chrono::milliseconds(10));

  // This one shouldn't have been hit yet, it isn't scheduled to be hit for 10s
  ASSERT_FALSE(*x) << "A delayed dispatch was invoked extremely early";

  // This one should have been ready almost at the same time as it was pended
  ASSERT_TRUE(*y) << "An out-of-order delayed dispatch was not executed in time as expected";
}

TEST_F(CoreThreadTest, VerifyTimedSort) {
  m_create->Initiate();
  AutoRequired<CoreThread> t;

  std::vector<size_t> v;

  // Pend a stack of lambdas.  Each lambda waits 3i milliseconds, and pushes the value
  // i to the back of a vector.  If the delay method is implemented correctly, the resulting
  // vector will always wind up sorted, no matter how we push elements to the queue.
  // To doubly verify this property, we don't trivially increment i from the minimum to the
  // maximum--rather, we use a simple PRNG called a linear congruential generator and hop around
  // the interval [1...12] instead.
  for(size_t i = 1; i != 0; i = (i * 5 + 1) % 16)
    *t += boost::chrono::milliseconds(i * 3), [&v, i] { v.push_back(i); };

  // Delay 50ms for the thread to finish up.  Technically this is 11ms more than we need.
  boost::this_thread::sleep_for(boost::chrono::milliseconds(75));

  // Verify that the resulting vector is sorted.
  ASSERT_TRUE(std::is_sorted(v.begin(), v.end())) << "A timed sort implementation did not generate a sorted sequence as expected";
}

TEST_F(CoreThreadTest, VerifyPendByTimePoint) {
  m_create->Initiate();
  AutoRequired<CoreThread> t;

  // Pend by an absolute time point, nothing really special here
  std::shared_ptr<bool> x(new bool(false));
  *t += (boost::chrono::high_resolution_clock::now() + boost::chrono::milliseconds(1)), [&t, x] {
    *x = true;
    t->Stop();
  };

  // Verify that this lambda does not run right away:
  ASSERT_FALSE(*x) << "A timepoint-based delayed dispatch was invoked early";

  // Verify that we hit this after one ms of delay
  ASSERT_TRUE(t->WaitFor(boost::chrono::seconds(5))) << "A timepoint-based delayed dispatch was not invoked in a timely fashion";
}

class WaitsALongTimeThenQuits:
  public CoreThread
{
public:
  WaitsALongTimeThenQuits(void):
    m_runExiting(false)
  {}

  bool m_runExiting;

  void Run(void) override {
    boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    m_runExiting = true;
  }
};

TEST_F(CoreThreadTest, NestedContextWait) {
  // Initiate the outer context:
  m_create->Initiate();

  // Create a subcontext which has our delay thread in it:
  AutoCreateContext ctxt;
  auto waitsAwhile = ctxt->Inject<WaitsALongTimeThenQuits>();
  ctxt->Initiate();

  // Stop and delay on the outer context:
  m_create->SignalShutdown();
  m_create->Wait();

  // Now we verify that our interior thread has actually quit:
  ASSERT_TRUE(!waitsAwhile->IsRunning()) << "Inner thread was marked running by CoreThread when outer context returned";
  ASSERT_TRUE(waitsAwhile->m_runExiting) << "Inner thread marked as stopped, but has not apparently quit";
}

TEST_F(CoreThreadTest, WaitBeforeInitiate) {
  // Wait for the outer context before it's actually initiated:
  ASSERT_FALSE(m_create->Wait(boost::chrono::seconds(0))) << "A wait operation on a context succeeded even though it was not yet started";

  // Stop the outer context without even starting it
  m_create->SignalTerminate();

  // Try to add a thread to the context.  This should cause the thread to be immediately marked as "stopped" and should allow a Wait
  // operation on the thread to return right away.
  AutoRequired<CoreThread> ct;
  ASSERT_TRUE(!ct->IsRunning()) << "A thread added to an already-stopped context was incorrectly marked as running";
  ASSERT_TRUE(ct->ShouldStop()) << "A thread added to an already-stopped context did not report that it should be stopped";
  ASSERT_TRUE(ct->WaitFor(boost::chrono::seconds(0))) << "A thread added to an already-stopped context did not correctly respond to a zero-duration wait";

  // Wait again.  This should suceed right away.
  ASSERT_TRUE(m_create->Wait(boost::chrono::seconds(0))) << "Failed to wait on a context which should have already been stopped";
}

template<ThreadPriority priority>
class JustIncrementsANumber:
  public CoreThread
{
public:
  JustIncrementsANumber():
    val(0)
  {}
  
  volatile int64_t val;

  // This will be a hotly contested conditional variable
  AutoRequired<boost::mutex> contended;

  void Run(void) override {
    ElevatePriority p(*this, priority);

    while(!ShouldStop()) {
      // Obtain the lock and then increment our value:
      boost::lock_guard<boost::mutex> lk(*contended);
      val++;
    }
  }
};

TEST_F(CoreThreadTest, ReentrantStopOnTeardown) {
  std::shared_ptr<CoreThread> ct = AutoRequired<CoreThread>();

  // Our thread will try to stop itself on teardown, this will cause it to trivially deadlock if any locks
  // are held while it passes notification to listeners:
  ct->AddTeardownListener([ct] {
    ct->Stop();
  });

  // Kickoff and then delay:
  m_create->Initiate();
  m_create->SignalShutdown();
  ASSERT_TRUE(m_create->Wait(boost::chrono::seconds(1))) << "Thread deadlocked when attempting to stop itself from a teardown listener";
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
  ctxt->Initiate();
  ASSERT_TRUE(ctxt->Wait(boost::chrono::seconds(5))) << "Context did not tear down in a timely fashion";
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

#ifdef _MSC_VER
#include "windows.h"

TEST_F(CoreThreadTest, VerifyCanBoostPriority) {
  // Create two spinners and kick them off at the same time:
  AutoRequired<JustIncrementsANumber<ThreadPriority::BelowNormal>> lower;
  AutoRequired<JustIncrementsANumber<ThreadPriority::Normal>> higher;
  m_create->Initiate();

  // We want all of our threads to run on ONE cpu for awhile, and then we want to put it back at exit
  SetProcessAffinityMask(GetCurrentProcess(), 1);
  auto onreturn = MakeAtExit([] {
    SetProcessAffinityMask(GetCurrentProcess(), ~0);
  });

  // Poke the conditional variable a lot:
  AutoRequired<boost::mutex> contended;
  for(size_t i = 100; i--;) {
    // We sleep while holding contention lock to force waiting threads into the sleep queue.  The reason we have to do
    // this is due to the way that mutex is implemented under the hood.  The STL mutex uses a high-frequency variable
    // and attempts to perform a CAS (check-and-set) on this variable.  If it succeeds, the lock is obtained; if it
    // fails, it will put the thread into a non-ready state by calling WaitForSingleObject on Windows or one of the
    // mutex_lock methods on Unix.
    //
    // When a thread can't be run, it's moved from the OS's ready queue to the sleep queue.  The scheduler knows that
    // the thread can be moved back to the ready queue if a particular object is signalled, but in the case of a lock,
    // only one of the threads waiting on the object can actually be moved to the ready queue.  It's at THIS POINT that
    // the operating system consults the thread priority--if only thread can be moved over, then the highest priority
    // thread will wind up in the ready queue every time.
    //
    // Thread priority does _not_ necessarily influence the amount of time the scheduler allocates allocated to a ready
    // thread with respect to other threads of the same process.  This is why we hold the lock for a full millisecond,
    // in order to force the thread over to the sleep queue and ensure that the priority resolution mechanism is
    // directly tested.
    boost::lock_guard<boost::mutex> lk(*contended);
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
  }

  // Need to terminate before we try running a comparison.
  m_create->SignalTerminate();

  ASSERT_LE(lower->val, higher->val) << "A lower-priority thread was moved out of the sleep queue more frequently than a high-priority thread";
}
#else
#pragma message "Warning:  SetThreadPriority not implemented on Unix"
#endif
