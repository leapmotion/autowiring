// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/CoreThread.h>
#include <autowiring/DispatchQueue.h>
#include <thread>
#include FUTURE_HEADER

using namespace std;

class DispatchQueueTest:
  public testing::Test,
  public DispatchQueue
{};

class EventMaker:
  public CoreThread
{
public:
  EventMaker():
    CoreThread("EventMaker")
  {}
};


template<int N>
class Thread:
  public CoreThread
{};


TEST_F(DispatchQueueTest, SimpleEvents) {
  int count = 0;

  *this += [&count] () {
    ++count;
  };

  *this += [&count] () {
    count += 5 ;
  };

  int num = DispatchAllEvents();

  *this += [&count]() {
    count += 5 ;
  };

  ASSERT_EQ(2, num);
  ASSERT_EQ(6, count);
}

TEST_F(DispatchQueueTest, PathologicalStartAndStop){
  AutoRequired<Thread<1>> t1;
  AutoRequired<Thread<2>> t2;
  AutoRequired<Thread<3>> t3;
  AutoRequired<Thread<4>> t4;
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  // We don't need a strong guarantee that these threads exit in a timely fashion, just that they
  // do so _eventually_.
  t1->Stop(true);
  ASSERT_TRUE(t1->WaitFor(std::chrono::seconds(10)));
  t2->Stop(true);
  ASSERT_TRUE(t2->WaitFor(std::chrono::seconds(10)));
  t3->Stop(true);
  ASSERT_TRUE(t3->WaitFor(std::chrono::seconds(10)));
  t4->Stop(true);
  ASSERT_TRUE(t4->WaitFor(std::chrono::seconds(10)));
}

TEST_F(DispatchQueueTest, TrivialBarrier) {
  AutoCurrentContext()->Initiate();
  AutoRequired<CoreThread> ct;

  ASSERT_TRUE(ct->Barrier(std::chrono::seconds(0))) << "Zero-time barrier on a zero-length queue did not pass as expected";
}

TEST_F(DispatchQueueTest, Barrier) {
  AutoCurrentContext()->Initiate();
  AutoRequired<CoreThread> ct;

  // Barrier on an empty dispatch queue should return right away:
  ASSERT_TRUE(ct->Barrier(std::chrono::seconds(5))) << "Barrier on an empty dispatch queue did not return immediately as expected";

  // Attach a lambda that will wait until the promise is complete:
  std::promise<bool> barrier;
  *ct += [&] {
    auto f = barrier.get_future();
    f.wait();
  };
  ASSERT_FALSE(ct->Barrier(std::chrono::milliseconds(1))) << "Barrier returned even though a dispatcher was not complete";
  barrier.set_value(true);

  // Now we should be able to complete:
  ASSERT_TRUE(ct->Barrier(std::chrono::seconds(5))) << "Barrier did not return even though a dispatcher should have completed";
}

TEST_F(DispatchQueueTest, BarrierWithSingleDelayedDispatcher) {
  AutoCurrentContext()->Initiate();
  AutoRequired<CoreThread> ct;

  // Hold our initial lock down so we can choose when the dispatcher concludes:
  auto b = std::make_shared<std::mutex>();
  std::unique_lock<std::mutex> lk(*b);

  // This dispatch entry will delay until we're ready for it to continue:
  *ct += [b] { std::unique_lock<std::mutex> lk(*b); };

  // Now we invoke the barrier with a timeout.  This barrier must fail because the above block
  // should prevent barriers from succeeding
  ASSERT_FALSE(ct->Barrier(std::chrono::microseconds(1))) << "Barrier passed even though an entry was still being processed";
}

struct BarrierMonitor {
  // Standard continuation behavior:
  std::mutex lock;
  std::condition_variable cv;
  size_t nReady = 0;
  bool done = false;
};

TEST_F(DispatchQueueTest, BarrierWithAbort) {
  AutoCurrentContext()->Initiate();
  AutoRequired<CoreThread> ct;

  // Barrier construction, hold down lock before kicking anything off so we can set everything up:
  auto b = std::make_shared<BarrierMonitor>();
  std::unique_lock<std::mutex> lk(b->lock);

  // This dispatch entry will delay until we're ready for it to continue:
  *ct += [b] {
    std::unique_lock<std::mutex> lk(b->lock);
    b->nReady++;
    b->cv.notify_all();
    b->cv.wait(lk, [&] { return b->done; });
  };

  // Launch something that will barrier:
  auto f = std::async(
    std::launch::async,
    [=] {
      std::unique_lock<std::mutex> { b->lock },
      b->nReady++;
      b->cv.notify_all();
      return ct->Barrier(std::chrono::seconds(5));
    }
  );

  // Wait for all threads to hit the barrier, then signal anyone interested that they can back out
  b->cv.wait(lk, [&] { return b->nReady == 2; });
  b->done = true;
  b->cv.notify_all();

  // Abandon the queue before relinquishing the lock:
  ct->Abort();

  ASSERT_EQ(std::future_status::ready, f.wait_for(std::chrono::seconds(5))) << "Barrier did not abort fast enough";
  ASSERT_ANY_THROW(f.get()) << "Barrier call returned instead of throwing an exception";
}

class DispatchQueueUnwrap:
  public DispatchQueue
{
public:
  using DispatchQueue::DispatchAllEvents;
};

TEST_F(DispatchQueueTest, AbortsWithinDispatcherDtor) {
  DispatchQueueUnwrap dq;
  {
    std::shared_ptr<bool> abortsOnTeardown(
      new bool,
      [&dq](bool* ptr) {
        // Pend a lambda and then immediately cause it to be destroyed
        dq += [] {};
        dq.Abort();
        delete ptr;
      }
    );
    dq += [abortsOnTeardown] {};
  }

  // Now try to run the dispatch behavior
  dq.DispatchAllEvents();
}


TEST_F(DispatchQueueTest, MoveConstructor) {
  int counter = 0;
  {
    DispatchQueue dq1;

    dq1 += [&counter] {counter++;};
    dq1 += [&counter] {counter++;};

    DispatchQueue dq2(std::move(dq1));

    dq2 += [&counter] {counter++;};

    dq2.DispatchAllEvents();
    ASSERT_EQ(counter, 3) << "Lambdas not trasfered by move constructor";

    dq2 += [&counter] {counter++;};
  }
  ASSERT_EQ(counter, 3) << "Lambdas not trasfered by move constructor";
}

TEST_F(DispatchQueueTest, ZeroIdenticalToPend) {
  DispatchQueue dq;

  int reference = 101;
  int observation = 1;
  dq += std::chrono::seconds(0), [&] { observation = reference; };
  dq += [&] { reference = 102; };
  ASSERT_EQ(2UL, dq.GetDispatchQueueLength()) << "Two dispatchers were added but two were not detected on the queue";

  // Verify that the lambdas were executed in order:
  dq.DispatchAllEvents();
  ASSERT_EQ(0UL, dq.GetDispatchQueueLength()) << "Not all dispatchers were executed even though all dispatchers should have been ready";
  ASSERT_NE(1, observation) << "Zero-delay lambda was not executed";
  ASSERT_EQ(101, observation) << "Zero-delay lambda did not run in the order it was pended";
}

TEST_F(DispatchQueueTest, AbortObserver) {
  DispatchQueue dq;
  dq += [] {};

  std::thread t{ [&dq] {
    try {
      for (;;)
        dq.WaitForEvent();
    }
    catch (dispatch_aborted_exception&) {}
  }};
  auto x = MakeAtExit([&] {
    dq.Abort();
    t.join();
  });

  // Pend a few operations, barrier for their completion, and then cancel:
  for (size_t i = 0; i < 100; i++)
    dq += [i] {};

  bool onAbortedCalled = false;
  dq.onAborted += [&] { onAbortedCalled = true; };
  dq.Abort();
  ASSERT_TRUE(onAbortedCalled) << "Abort signal handler not asserted as expected";
}

TEST_F(DispatchQueueTest, TryDispatchTest) {
  DispatchQueue dq;
  dq += [] {
    throw std::runtime_error{"Error!"};
  };
  ASSERT_THROW(dq.TryDispatchEvent(), std::runtime_error);
  ASSERT_EQ(1UL, dq.GetDispatchQueueLength());
}

TEST_F(DispatchQueueTest, VerifyRetry) {
  DispatchQueue dq;
  size_t nCalled = 0;
  dq += [&nCalled] {
    if(!nCalled++)
      throw std::runtime_error{ "Error!" };
  };
  ASSERT_THROW(dq.TryDispatchEvent(), std::runtime_error);
  ASSERT_EQ(1U, dq.GetDispatchQueueLength());
  ASSERT_TRUE(dq.TryDispatchEvent());
  ASSERT_EQ(2U, nCalled);
}

TEST_F(DispatchQueueTest, TwoDispatchRetry) {
  DispatchQueue dq;
  size_t nCalled = 0;
  dq += [&nCalled] {
    if (!nCalled++)
      throw std::runtime_error{ "Error!" };
  };
  dq += [] {};
  ASSERT_THROW(dq.TryDispatchEvent(), std::runtime_error);
  ASSERT_EQ(2U, dq.GetDispatchQueueLength());
  ASSERT_TRUE(dq.TryDispatchEvent());
  ASSERT_EQ(1U, dq.GetDispatchQueueLength());
  ASSERT_TRUE(dq.TryDispatchEvent());
  ASSERT_EQ(2U, nCalled);
  ASSERT_EQ(0U, dq.GetDispatchQueueLength());
}

TEST_F(DispatchQueueTest, SimpleCancel) {
  DispatchQueue dq;
  auto called = std::make_shared<bool>(false);
  dq += [called] { *called = true; };
  ASSERT_TRUE(dq.Cancel()) << "Dispatch queue failed to cancel a lambda as expected";
  ASSERT_FALSE(dq.DispatchEvent()) << "Succeeded in dispatching an event that should not have been dispatched";
  ASSERT_FALSE(*called) << "Dispatch queue executed a lambda that should have been destroyed";
  ASSERT_TRUE(called.unique()) << "Dispatch queue leaked a lambda function";
}

TEST_F(DispatchQueueTest, TimedCancel) {
  DispatchQueue dq;

  auto called1 = std::make_shared<bool>(false);
  dq += std::chrono::hours(20), [called1] { *called1 = true; };

  auto called2 = std::make_shared<bool>(false);
  dq += std::chrono::hours(12), [called2] { *called2 = true; };

  auto called3 = std::make_shared<bool>(false);
  dq += std::chrono::microseconds(1), [called3] { *called3 = true; };

  auto called4 = std::make_shared<bool>(false);
  dq += std::chrono::microseconds(1), [called4] { *called4 = true; };

  ASSERT_TRUE(dq.Cancel());
  ASSERT_TRUE(dq.WaitForEvent(std::chrono::seconds(30))) << "Cancel removed the wrong event from the queue";
  ASSERT_TRUE(*called4) << "Cancel did not drop the most immanent dispatcher";
  ASSERT_FALSE(*called3) << "Cancel did not drop the most immanent dispatcher";
  ASSERT_TRUE(dq.Cancel());
  ASSERT_TRUE(dq.Cancel());
  ASSERT_EQ(0U, dq.GetDispatchQueueLength()) << "Dispatch queue was not empty even though it should have been";
  ASSERT_TRUE(called1.unique() && called2.unique() && called3.unique() && called4.unique()) << "Cancellation leaked some lambdas";
}

TEST_F(DispatchQueueTest, MixedCancel) {
  DispatchQueue dq;

  auto called1 = std::make_shared<bool>(false);
  dq += std::chrono::minutes(30), [called1] { *called1 = true; };

  auto called2 = std::make_shared<bool>(false);
  dq += [called2] { *called2 = true; };

  ASSERT_TRUE(dq.Cancel());
  ASSERT_FALSE(called1.unique()) << "Cancellation cancelled the wrong lambda";
  ASSERT_TRUE(called2.unique()) << "Cancellation cancelled the wrong lambda";
}

TEST_F(DispatchQueueTest, DelayedAbort) {
  DispatchQueue dq;

  auto v = std::make_shared<bool>(true);
  dq += std::chrono::hours{ 1 }, [v] {};
  dq.Abort();
  ASSERT_TRUE(v.unique()) << "A delayed dispatcher was leaked after a call to Abort";
}
