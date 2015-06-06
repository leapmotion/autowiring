// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/Autowired.h>
#include <autowiring/BasicThread.h>
#include <autowiring/CoreThread.h>
#include <autowiring/thread_specific_ptr.h>
#include THREAD_HEADER

class AutowiringUtilitiesTest:
  public testing::Test
{};

static autowiring::thread_specific_ptr<int> s_thread_specific_int;

class Thread1:
  public BasicThread
{
  virtual void Run(){
    s_thread_specific_int.reset(new int(4));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(4, *s_thread_specific_int);
  }
};

class Thread2:
  public BasicThread
{
  virtual void Run() {
    s_thread_specific_int.reset(new int(3));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(3, *s_thread_specific_int);
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
  
  ASSERT_EQ(5, *s_thread_specific_int);
  AutoCurrentContext()->SignalShutdown(true);
}
