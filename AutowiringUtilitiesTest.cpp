#include "stdafx.h"
#include "AutowiringUtilitiesTest.h"
#include "Autowired.h"
#include "BasicThread.h"
#include "CoreThread.h"
#include "thread_specific_ptr.h"
#include "TestFixtures/ThreadBarrier.h"
#include THREAD_HEADER

static leap::thread_specific_ptr<int> s_thread_specific_int;

class Thread1:
  public BasicThread
{
  virtual void Run(){
    s_thread_specific_int.reset(new int(4));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(4, *s_thread_specific_int);
  }
};

class Thread2:
  public BasicThread
{
  virtual void Run() {
    s_thread_specific_int.reset(new int(3));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(3, *s_thread_specific_int);
  }
};


TEST_F(AutowiringUtilitiesTest, ThreadSpecificPtr) {
  AutoCreateContext ctxt;
  CurrentContextPusher pshr(ctxt);
  
  AutoRequired<Thread1> thread1;
  AutoRequired<Thread2> thread2;
  
  s_thread_specific_int.reset(new int(5));
  
  ctxt->Initiate();
  
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  
  EXPECT_EQ(5, *s_thread_specific_int);
  AutoCurrentContext()->SignalShutdown(true);
}

template<int N>
class Thread:
  public CoreThread
{};

TEST_F(AutowiringUtilitiesTest, ThreadBarrierTest) {
  AutoCurrentContext()->Initiate();
  
  ThreadBarrier barr(3);
  ThreadBarrier resultBarr(2);
  AutoRequired<Thread<1>> thread1;
  AutoRequired<Thread<2>> thread2;
  
  // Make sure barrier waits for 3 threads
  
  bool test = false;
  
  *thread1 += [&barr, &test, &resultBarr] {
    barr.wait();
    test = true;
    resultBarr.wait();
  };
  
  *thread2 += [&barr, &test] {
    EXPECT_FALSE(test);
    barr.wait();
  };
  
  EXPECT_FALSE(test);
  barr.wait();
  resultBarr.wait();
  EXPECT_TRUE(test);
  
  // Make sure reset happens correctly
  bool first = false;
  bool second = false;
  *thread1 += [&barr, &first]{
    first = true;
    barr.wait();
  };
  
  *thread2 += [&barr, &second]{
    second = true;
    barr.wait();
  };
  
  barr.wait();
  EXPECT_TRUE(first && second);
  AutoCurrentContext()->SignalShutdown(true);
}
