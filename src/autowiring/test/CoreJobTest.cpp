// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/CoreJob.h>
#include THREAD_HEADER
#include ARRAY_HEADER

class CoreJobTest:
  public testing::Test
{};

TEST_F(CoreJobTest, VerifySimpleProperties) {
  AutoRequired<CoreJob> jb;
  AutoCurrentContext ctxt;

  ASSERT_FALSE(ctxt->IsInitiated()) << "CoreJob reported it could receive events before its enclosing context was created";

  // Create a thread which will delay for acceptance, and then quit:
  std::mutex lock;
  std::condition_variable cv;
  auto done = std::make_shared<bool>(false);
  std::thread t([&] {
    ctxt->DelayUntilInitiated();

    (std::lock_guard<std::mutex>)lock,
    *done = true,
    cv.notify_all();
  });

  auto wait = [&] (std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lk(lock);
    return cv.wait_for(lk, timeout, [&] { return *done; });
  };

  // Verify that this thread doesn't back out right away:
  ASSERT_FALSE(wait(std::chrono::milliseconds(10))) << "CoreJob did not block a client who was waiting for its readiness to accept dispatchers";

  // Now start the context and verify that certain properties changed as anticipated:
  ctxt->Initiate();
  ASSERT_TRUE(ctxt->DelayUntilInitiated()) << "CoreJob did not correctly delay for dispatch acceptance";

  // Verify that the blocked thread has become unblocked and quits properly:
  ASSERT_TRUE(wait(std::chrono::seconds(1))) << "CoreJob did not correctly signal a blocked thread that it was ready to accept dispatchers";

  ctxt->SignalShutdown(true);
  t.join();
}

TEST_F(CoreJobTest, VerifySimpleSubmission) {
  AutoRequired<CoreJob> jb;

  auto myFlag = std::make_shared<bool>(false);
  *jb += [myFlag] {
    *myFlag = true;
  };

  // Kickoff, signal for a shutdown to take place, and then verify the flag
  AutoCurrentContext ctxt;
  ctxt->Initiate();
  ctxt->SignalShutdown(true);
  ASSERT_TRUE(*myFlag) << "CoreJob did not properly execute its thread";
}

TEST_F(CoreJobTest, VerifyTeardown) {
  AutoRequired<CoreJob> job;
  AutoCurrentContext ctxt;

  bool check1 = false;
  bool check2 = false;
  bool check3 = false;

  *job += [&check1] {
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    check1 = true;
  };
  *job += [&check2] {
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    check2 = true;
  };
  ctxt->Initiate();
  *job += [&check3] {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    check3 = true;
  };

  ctxt->SignalShutdown(true);
  ASSERT_TRUE(check1) << "Lambda 1 didn't finish";
  ASSERT_TRUE(check2) << "Lambda 2 didn't finish";
  ASSERT_TRUE(check3) << "Lambda 3 didn't finish";
}

TEST_F(CoreJobTest, AbandonedDispatchers) {
  auto v = std::make_shared<bool>(false);

  AutoRequired<CoreJob> cj;
  *cj += [v] { *v = true; };

  // Graceful shutdown on our enclosing context without starting it:
  AutoCurrentContext()->SignalShutdown(true);

  // Verify that all lambdas on the CoreThread got called as expected:
  ASSERT_FALSE(*v) << "Lambdas attached to a CoreJob should not be executed when the enclosing context is terminated without being started";
}

TEST_F(CoreJobTest, RecursiveAdd) {
  bool first = false;
  bool second = false;
  bool third = false;

  AutoRequired<CoreJob> cj;

  AutoCurrentContext()->Initiate();

  *cj += [&first,&second,&third, &cj] {
    first = true;
    *cj += [&first,&second,&third,&cj] {
      second = true;
      *cj += [&first,&second,&third,&cj] {
        third = true;
      };
      cj->Stop(true);
    };
  };

  cj->Wait();

  // Verify that all lambdas on the CoreThread got called as expected:
  ASSERT_TRUE(first) << "Appended lambda didn't set value";
  ASSERT_TRUE(second) << "Appended lambda didn't set value";
  ASSERT_TRUE(third) << "Appended lambda didn't set value";
}

TEST_F(CoreJobTest, RaceCondition) {
  AutoCurrentContext()->Initiate();

  for (int i=0; i<5; i++) {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);
    AutoRequired<CoreJob> cj;
    ctxt->Initiate();

    bool first = false;
    bool second = false;

    *cj += [&first] {
      first = true;
    };

    std::this_thread::sleep_for(std::chrono::milliseconds(i));

    *cj += [&second, &cj] {
      second = true;
    };

    ctxt->SignalShutdown(true);

    ASSERT_TRUE(first) << "Failed after set value in lambda";
    ASSERT_TRUE(second) << "Failed to set value when delayed " << i << " milliseconds";
  }
}

TEST_F(CoreJobTest, CorrectlyAssignedCurrentContext) {
  AutoCurrentContext()->Initiate();
  AutoRequired<CoreJob> job;

  std::shared_ptr<CoreContext> ctxt;
  *job += [&ctxt] { ctxt = AutoCurrentContext(); };
  *job += [job] { job->Stop(true); };
  ASSERT_TRUE(job->WaitFor(std::chrono::seconds(5)));

  // Now verify that the job was run in the right thread context:
  ASSERT_EQ(AutoCurrentContext(), ctxt) << "Job lambda was not run with the correct CoreContext current";
}

TEST_F(CoreJobTest, RecursiveDeadlock) {
  // Create a CoreJob which will wait for a bit.  Then, delegate its deletion responsibility to the thread
  // itself, a
  AutoCreateContext ctxt;
  AutoRequired<CoreJob> cj(ctxt);
  *cj += [] {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  };
}

TEST_F(CoreJobTest, PendFromMultipleThreads) {
  AutoCurrentContext ctxt;
  AutoRequired<CoreJob> cj;
  std::array<std::thread, 4> threads;
  const size_t times{ 256 };
  int counter{ 0 };

  for (size_t i = 0; i < threads.size(); i++) {
    threads[i] = std::thread([&] {
      ctxt->DelayUntilInitiated();
      for (int j = 0; j < times; j++) {
        *cj += [&counter] {
          counter++; // Should be updated exclusively in the CoreJob's thread
        };
      }
    });
  }
  ctxt->Initiate();
  for (size_t i = 0; i < threads.size(); i++) {
    if (threads[i].joinable())
      threads[i].join();
  }
  ctxt->SignalShutdown(true);
  ASSERT_EQ(times*threads.size(), counter);
}
