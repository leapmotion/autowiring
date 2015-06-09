// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/autowiring.h>
#include <autowiring/ManualThreadPool.h>
#include <autowiring/NullPool.h>
#include <autowiring/SystemThreadPoolStl.h>
#include FUTURE_HEADER

class ThreadPoolTest:
  public testing::Test
{};

TEST_F(ThreadPoolTest, SimpleSubmission) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  // Simple validation
  auto pool = ctxt->GetThreadPool();
  ASSERT_NE(nullptr, pool.get()) << "Pool can never be null on an initiated context";
  ASSERT_EQ(nullptr, dynamic_cast<autowiring::NullPool*>(pool.get())) << "After context initiation, the pool should not be a null pool";
  ASSERT_TRUE(pool->IsStarted()) << "Pool was not started when the enclosing context was initiated";

  // Submit a job and then block for its completion.  Use a promise to ensure that
  // the job is being executed in some other thread context.
  auto p = std::make_shared<std::promise<void>>();

  *ctxt += [&] {
    p->set_value();
  };

  auto rs = p->get_future();
  ASSERT_EQ(std::future_status::ready, rs.wait_for(std::chrono::seconds(5))) << "Thread pool lambda was not dispatched in a timely fashion";
}

static void PoolOverload(void) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  size_t cap = 10000;
  auto ctr = std::make_shared<std::atomic<size_t>>(cap);
  auto p = std::make_shared<std::promise<void>>();

  for (size_t i = cap; i--;)
    *ctxt += [=] {
      if (!--*ctr)
        p->set_value();
    };

  auto rs = p->get_future();
  ASSERT_EQ(std::future_status::ready, rs.wait_for(std::chrono::seconds(5))) << "Pool saturation did not complete in a timely fashion";
}

TEST_F(ThreadPoolTest, PoolOverload) {
  ::PoolOverload();
}

// On systems that don't have any OS-specific thread pool customizations, this method is redundant
// On systems that do, this method ensures parity of behavior
TEST_F(ThreadPoolTest, StlPoolTest) {
  AutoCurrentContext ctxt;
  auto pool = std::make_shared<autowiring::SystemThreadPoolStl>();
  ctxt->SetThreadPool(pool);
  pool->SuggestThreadPoolSize(2);
  ::PoolOverload();
}

TEST_F(ThreadPoolTest, PendBeforeContextStart) {
  AutoCurrentContext ctxt;

  // Pend
  auto barr = std::make_shared<std::promise<void>>();
  *ctxt += [barr] { barr->set_value(); };
  ASSERT_EQ(2UL, barr.use_count()) << "Lambda was not correctly captured by an uninitiated context";

  std::future<void> f = barr->get_future();
  ASSERT_EQ(std::future_status::timeout, f.wait_for(std::chrono::milliseconds(1))) << "A pended lambda was completed before the owning context was intiated";

  ctxt->Initiate();
  ASSERT_EQ(std::future_status::ready, f.wait_for(std::chrono::seconds(5))) << "A lambda did not complete in a timely fashion after its context was started";

  // Terminate, verify that we don't capture any more lambdas:
  ctxt->SignalShutdown();
  ASSERT_FALSE(*ctxt += [barr] {}) << "Lambda append operation incorrectly evaluated to true";
  ASSERT_TRUE(barr.unique()) << "Lambda was incorrectly captured by a context that was already terminated";
}

TEST_F(ThreadPoolTest, ManualThreadPoolBehavior) {
  // Make the manual pool that will be used for this test:
  auto pool = std::make_shared<autowiring::ManualThreadPool>();

  // Launch a thread that will join the pool:
  std::promise<std::shared_ptr<autowiring::ThreadPoolToken>> val;
  auto launch = std::async(
    std::launch::async,
    [pool, &val] {
      auto token = pool->PrepareJoin();
      val.set_value(token);
      pool->Join(token);
    }
  );

  auto valFuture = val.get_future();
  ASSERT_EQ(std::future_status::ready, valFuture.wait_for(std::chrono::seconds(5))) << "Join thread took too much time to start up";
  auto token = valFuture.get();

  // Set up the context 
  AutoCurrentContext ctxt;
  ctxt->SetThreadPool(pool);
  ctxt->Initiate();

  // Pend some lambdas to be executed by our worker thread:
  std::promise<void> hitDone;
  std::atomic<int> hitCount{10};
  for (size_t i = hitCount; i--; )
    *ctxt += [&] {
      if (!--hitCount)
        hitDone.set_value();
    };

  // Wait for everything to get hit:
  auto hitDoneFuture = hitDone.get_future();
  ASSERT_EQ(std::future_status::ready, hitDoneFuture.wait_for(std::chrono::seconds(5))) << "Manual pool did not dispatch lambdas in a timely fashion";

  // Verify that cancellation of the token causes the manual thread to back out
  token->Leave();
  ASSERT_EQ(std::future_status::ready, launch.wait_for(std::chrono::seconds(5))) << "Token cancellation did not correctly release a single waiting thread";
}
