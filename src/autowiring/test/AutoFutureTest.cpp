// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/auto_future.h>
#include <autowiring/autowiring.h>
#include <future>

class AutoFutureTest:
  public testing::Test
{};

TEST_F(AutoFutureTest, VerifyFutureExtraction) {
  auto f = std::async(
    std::launch::async,
    [] {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      return 5;
    }
  );

  auto pr = std::make_shared<std::promise<int>>();
  autowiring::then(
    f,
    [pr] { pr->set_value(201);}
  );

  // Wait for our async launch to conclude
  f.get();

  // Continuation task checks
  auto future = pr->get_future();
  ASSERT_EQ(
    std::future_status::ready,
    future.wait_for(std::chrono::seconds(5))
  ) << "Continuation task did not launch as expected when the main task concluded";
  ASSERT_EQ(201, future.get()) << "Future value not propagated correctly back to the origin";
}

TEST_F(AutoFutureTest, CorrectDestructionTest) {
  auto captured = std::make_shared<bool>(false);
  {
    auto f = std::async(
      std::launch::deferred,
      [] {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        return 5;
      }
    );

    autowiring::then(f, [captured] {
      *captured = true;
    });
    ASSERT_FALSE(captured.unique()) << "Lambda was destroyed prematurely";
    f.get();
    ASSERT_TRUE(captured.unique()) << "Continuation lambda leaked memory";
  }

  ASSERT_TRUE(*captured) << "Continuation lambda did not run as expected";
}
