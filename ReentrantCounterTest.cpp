#include "stdafx.h"
#include "ReentrantCounterTest.h"
#include "ReentrantCounter.h"
#include <boost/chrono.hpp>
#include <vector>

// Timeouts, in microseconds:
const size_t gc_timeouts[] = {500, 221, 3700, 1910, 21100, 442, 9281, 7201, 49102, 701, 222};

// Recovered timeouts:
PerformanceCounter g_timeouts[ARRAYCOUNT(gc_timeouts)];

// Current offset:
size_t g_curOffset = 0;

// Slack value--we allow 50us of slack
const boost::chrono::microseconds slack(50);

// Reentrant function proper:
void Reenter() {
  // Start our counter:
  ReentrantCounter ctr(g_timeouts[g_curOffset]);

  // Spin until time-up.  We want precision, so we spin rather than sleep
  auto finish = boost::chrono::high_resolution_clock::now() + boost::chrono::microseconds(gc_timeouts[g_curOffset]);
  while(boost::chrono::high_resolution_clock::now() < finish);

  // Reenter every second value:
  if(g_curOffset++ % 2)
    Reenter();
}

TEST_F(ReentrantCounterTest, VerifySimpleBenchmarking) {
  // Total time we will spin for, to test reentrancy:
  const boost::chrono::microseconds spinTime(1299);

  PerformanceCounter counter;

  // Wait for spin-time period:
  {
    ReentrantCounter ctr(counter);
    auto finish = boost::chrono::high_resolution_clock::now() + spinTime;
    while(boost::chrono::high_resolution_clock::now() < finish);
  }

  // Verify we hit exactly once:
  EXPECT_EQ(1, counter.hitCount) << "Performance counter incremented an incorrect number of times";

  // Verify we are in range:
  EXPECT_LT(spinTime - slack, counter.lingerTime) << "Reentrant counter underestimated the total spin time";
  EXPECT_GT(spinTime + slack, counter.lingerTime) << "Reentrant counter overestimated the total spin time";
}

TEST_F(ReentrantCounterTest, VerifyComplexReentrancy) {
  // Run our reentrant function:
  while(g_curOffset < ARRAYCOUNT(gc_timeouts)){
    //the dingus who wrote this ended up creating SEVERAL ROOTS so we have to have a way to mark fresh runs
    ReentrantCounter::globalTimeElapsedSinceStart = boost::chrono::high_resolution_clock::now() - boost::chrono::high_resolution_clock::now();
    Reenter();
  }
  // Validate timeouts:;
  for(size_t i = 0; i < ARRAYCOUNT(gc_timeouts); i++) {
    EXPECT_EQ(1, g_timeouts[i].hitCount) << "Performance counter hit too many times";
    EXPECT_LT(
      g_timeouts[i].lingerTime - slack,
      boost::chrono::microseconds(gc_timeouts[i])
    );
    EXPECT_GT(
      g_timeouts[i].lingerTime + slack,
      boost::chrono::microseconds(gc_timeouts[i])
    );
  }
}