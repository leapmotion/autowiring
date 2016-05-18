// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/BasicThread.h>
#include FUTURE_HEADER

using namespace std::chrono;

class BasicThreadTest:
  public testing::Test
{};

class SpinsAndThenQuits:
  public BasicThread
{
public:
  SpinsAndThenQuits(void) :
    BasicThread("SpinsAndThenQuits")
  {}

  milliseconds m_spinDelayTime;
  size_t spinCount = 0;

  bool m_continue = false;
  std::condition_variable m_signal;

  std::chrono::milliseconds m_kernelTime;
  std::chrono::milliseconds m_userTime;

  void Continue(void) {
    this->PerformStatusUpdate([this] {
      m_continue = true;
    });
  }

  void Run(void) override {
    WaitForStateUpdate([this] { return m_continue; });
    auto endTime = steady_clock::now() + m_spinDelayTime;
    while (steady_clock::now() < endTime)
      spinCount++;
    GetThreadTimes(m_kernelTime, m_userTime);
  }
};

TEST_F(BasicThreadTest, ValidateThreadTimes) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  auto spinsThenQuits = ctxt->Inject<SpinsAndThenQuits>();

  // By this point, not much should have happened:
  std::chrono::milliseconds kernelTime;
  std::chrono::milliseconds userTime;
  spinsThenQuits->GetThreadTimes(kernelTime, userTime);

  // Kick off the thread and wait for it to exit:
  spinsThenQuits->Continue();
  ASSERT_TRUE(spinsThenQuits->WaitFor(std::chrono::seconds(10))) << "Spin-then-quit test took too long to execute";

  // Ensure the runtime matches our expectation
  ASSERT_LE(spinsThenQuits->m_spinDelayTime / 2, userTime) <<
    "Reported execution time could not possibly be correct, spin operation took less time to execute than should have been possible with the CPU";
}

TEST_F(BasicThreadTest, IsMainThread) {
  ASSERT_TRUE(BasicThread::IsMainThread()) << "Main thread not correctly identified as the main thread";
  std::future<bool> secondaryIsMain = std::async(
    std::launch::async,
    &BasicThread::IsMainThread
  );
  ASSERT_FALSE(secondaryIsMain.get()) << "Secondary thread incorrectly identified as the main thread";
}
