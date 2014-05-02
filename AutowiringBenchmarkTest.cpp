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

  EXPECT_GT(baseline * 3. / 2., benchmark) << "Average time to autowire one member was more than 3/2 of a hash map lookup";
}

template<int N>
struct dummy:
  public virtual ContextMember
{};

void InjectDummy(void) {
  Autowired<dummy<1>>();
  Autowired<dummy<2>>();
  Autowired<dummy<3>>();
  Autowired<dummy<4>>();
  Autowired<dummy<5>>();
  Autowired<dummy<6>>();
  Autowired<dummy<7>>();
  Autowired<dummy<8>>();
  Autowired<dummy<9>>();
  Autowired<dummy<10>>();
  Autowired<dummy<11>>();
  Autowired<dummy<12>>();
  Autowired<dummy<13>>();
  Autowired<dummy<14>>();
  Autowired<dummy<15>>();
  Autowired<dummy<16>>();
  Autowired<dummy<17>>();
  Autowired<dummy<18>>();
  Autowired<dummy<19>>();
  Autowired<dummy<20>>();
  Autowired<dummy<21>>();
  Autowired<dummy<22>>();
  Autowired<dummy<23>>();
  Autowired<dummy<24>>();
  Autowired<dummy<25>>();
};

#if defined(__GNUC__) && !defined(__clang__)
TEST_F(AutowiringBenchmarkTest, DISABLED_VerifyAutowiringCache)
#else
TEST_F(AutowiringBenchmarkTest, VerifyAutowiringCache)
#endif
{
  boost::chrono::nanoseconds baseline(0);
  boost::chrono::nanoseconds benchmark(0);

  for(int i = 0; i<100; ++i) {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);

    auto startBase = boost::chrono::high_resolution_clock::now();
    InjectDummy();
    baseline += boost::chrono::high_resolution_clock::now() - startBase;

    auto startBench = boost::chrono::high_resolution_clock::now();
    InjectDummy();
    benchmark += boost::chrono::high_resolution_clock::now() - startBench;
  }
  
  EXPECT_GT(baseline, benchmark) << "Autowiring cache not improving performance on subsequent autowirings";
}


TEST_F(AutowiringBenchmarkTest, VerifyAutowiredFast) {
  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);
    
    AutoRequired<dummy<1>> foo;
    AutowiredFast<dummy<1>> bar;
    
    ASSERT_TRUE(foo) << "AutoRequred member not created";
    EXPECT_TRUE(bar) << "AutowiredFast not satisfied";
  }
  
  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);
  
    AutowiredFast<dummy<1>> fast;
    Autowired<dummy<1>> wired;
  
    AutoRequired<dummy<1>> required;
  
    ASSERT_TRUE(required) << "AutoRequired member not created";
  
    EXPECT_TRUE(wired.IsAutowired()) << "Deferred Autowiring wasn't satisfied";
    EXPECT_FALSE(fast) << "AutowiredFast member was deferred";
  }
}

TEST_F(AutowiringBenchmarkTest, VerifyAutowiredFastPerformance) {
  boost::chrono::nanoseconds baseline(0);
  boost::chrono::nanoseconds benchmark(0);
  {
    // All of these tests will operate in the same context:
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);

    // Prime all memos:
    InjectDummy();

    // Test simple autowiring first:
    auto startBase = boost::chrono::high_resolution_clock::now();
    for(int i = 0; i < 500; ++i)
      InjectDummy();
    baseline += boost::chrono::high_resolution_clock::now() - startBase;
    
    auto startBench = boost::chrono::high_resolution_clock::now();
    for(int i = 0; i < 500; ++i) {
      AutowiredFast<dummy<1>>();
      AutowiredFast<dummy<2>>();
      AutowiredFast<dummy<3>>();
      AutowiredFast<dummy<4>>();
      AutowiredFast<dummy<5>>();
      AutowiredFast<dummy<6>>();
      AutowiredFast<dummy<7>>();
      AutowiredFast<dummy<8>>();
      AutowiredFast<dummy<9>>();
      AutowiredFast<dummy<10>>();
      AutowiredFast<dummy<11>>();
      AutowiredFast<dummy<12>>();
      AutowiredFast<dummy<13>>();
      AutowiredFast<dummy<14>>();
      AutowiredFast<dummy<15>>();
      AutowiredFast<dummy<16>>();
      AutowiredFast<dummy<17>>();
      AutowiredFast<dummy<18>>();
      AutowiredFast<dummy<19>>();
      AutowiredFast<dummy<20>>();
    }
    benchmark += boost::chrono::high_resolution_clock::now() - startBench;
  }
  
  EXPECT_GT(baseline, benchmark*1.75) << "Fast autowiring is slower than ordinary autowiring";
}
