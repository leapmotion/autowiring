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
  
  EXPECT_EQ(2, num);
  EXPECT_EQ(6, count);
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
      {
        std::unique_lock<std::mutex> lk(b->lock);
        b->nReady++;
        b->cv.notify_all();
      }
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
  bool rs;
  ASSERT_ANY_THROW(rs = f.get()) << "Barrier call returned " << std::boolalpha << rs << " instead of throwing an exception";
}
