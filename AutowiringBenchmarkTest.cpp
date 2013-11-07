#include "stdafx.h"
#include "AutowiringBenchmarkTest.h"
#include "TestFixtures/SimpleObject.h"
#include <boost/chrono/system_clocks.hpp>

TEST_F(AutowiringBenchmarkTest, VerifySimplePerformance) {
  const size_t n = 10000;

  // Insert the object:
  AutoRequired<SimpleObject>();

  // Time n hash map hits, in order to get a baseline:
  std::unordered_map<int, int> ref;
  ref[0] = 212;
  ref[1] = 21;

  boost::chrono::nanoseconds baseline;
  {
    auto start = boost::chrono::high_resolution_clock::now();
    for(size_t i = n; i--;)
      ref[i % 2];
    baseline = boost::chrono::high_resolution_clock::now() - start;
  }

  // Time n autowirings:
  boost::chrono::nanoseconds benchmark;
  {
    auto start = boost::chrono::high_resolution_clock::now();
    for(size_t i = n; i--;)
      Autowired<SimpleObject>();
    benchmark = (boost::chrono::high_resolution_clock::now() - start) / n;
  }

  EXPECT_GT(baseline * 3 / 2, benchmark) << "Average time to autowire one member was more than 3/2 of a hash map lookup";
}