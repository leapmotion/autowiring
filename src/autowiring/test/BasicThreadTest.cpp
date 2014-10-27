// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/BasicThread.h>

class BasicThreadTest:
  public testing::Test
{};

class SpinsAndThenQuits:
  public BasicThread
{
public:
  SpinsAndThenQuits(size_t spinCount) :
    BasicThread("SpinsAndThenQuits"),
    m_spinCount(spinCount),
    m_continue(false)
  {}

  volatile size_t m_spinCount;

  bool m_continue;
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
    while(m_spinCount--);
    GetThreadTimes(m_kernelTime, m_userTime);
  }
};

TEST_F(BasicThreadTest, ValidateThreadTimes) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  static const size_t spinCount = 10000000;
  auto spinsThenQuits = ctxt->Inject<SpinsAndThenQuits>(spinCount);

  // Instantaneous benchmark on the time it takes to decrement the counter value:
  std::chrono::nanoseconds benchmark;
  {
    auto startTime = std::chrono::high_resolution_clock::now();
    for(volatile size_t i = spinCount; i--;);
    benchmark = std::chrono::high_resolution_clock::now() - startTime;
  }

  // By this point, not much should have happened:
  std::chrono::milliseconds kernelTime;
  std::chrono::milliseconds userTime;
  spinsThenQuits->GetThreadTimes(kernelTime, userTime);

  // Kick off the thread and wait for it to exit:
  spinsThenQuits->Continue();
  ASSERT_TRUE(spinsThenQuits->WaitFor(std::chrono::seconds(10))) << "Spin-then-quit test took too long to execute";

  // Thread should not have been able to complete in less time than we completed, by a factor of ten or so at least
  ASSERT_LE(benchmark, spinsThenQuits->m_userTime * 10) <<
    "Reported execution time could not possibly be correct, spin operation took less time to execute than should have been possible with the CPU";
}