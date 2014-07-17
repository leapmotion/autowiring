#include "stdafx.h"
#include "BasicThreadTest.h"
#include "BasicThread.h"

class SpinsAndThenQuits:
  public BasicThread
{
public:
  SpinsAndThenQuits(size_t spinCount) :
    BasicThread("SpinsAndThenQuits"),
    m_spinCount(spinCount)
  {}

  volatile size_t m_spinCount;

  boost::chrono::nanoseconds m_kernelTime;
  boost::chrono::nanoseconds m_userTime;

  void Run(void) override {
    while(m_spinCount--);
    GetThreadTimes(m_kernelTime, m_userTime);
  }
};

TEST_F(BasicThreadTest, ValidateThreadTimes) {
  AutoCurrentContext ctxt;
  static const size_t spinCount = 10000000;
  auto spinsThenQuits = ctxt->Construct<SpinsAndThenQuits>(spinCount);

  // Instantaneous benchmark on the time it takes to decrement the counter value:
  boost::chrono::nanoseconds benchmark;
  {
    auto startTime = boost::chrono::high_resolution_clock::now();
    for(volatile size_t i = spinCount; i--;);
    benchmark = boost::chrono::high_resolution_clock::now() - startTime;
  }

  ctxt->Initiate();
  ASSERT_TRUE(spinsThenQuits->WaitFor(boost::chrono::seconds(10))) << "Spin-then-quit test took too long to execute";

  // Thread should not have been able to complete in less time than we completed, by a factor of ten or so at least
  ASSERT_LE(benchmark, spinsThenQuits->m_userTime * 10) <<
    "Reported execution time could not possibly be correct, spin operation took less time to execute than should have been possible with the CPU";
}