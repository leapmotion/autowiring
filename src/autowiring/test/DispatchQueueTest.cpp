// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/CoreThread.h>
#include <autowiring/DispatchQueue.h>
#include <future>

using namespace std;

class DispatchQueueTest:
  public testing::Test,
  public DispatchQueue
{};

class EventMaker:
  public CoreThread
{
public:
  EventMaker():
    CoreThread("EventMaker")
  {}
};


template<int N>
class Thread:
  public CoreThread
{};


TEST_F(DispatchQueueTest, SimpleEvents) {
  int count = 0;
  
  *this += [&count] () {
    ++count;
  };
  
  *this += [&count] () {
    count += 5 ;
  };
  
  int num = DispatchAllEvents();
  
  *this += [&count] () {
    count += 5 ;
  };
  
  EXPECT_EQ(2, num);
  EXPECT_EQ(6, count);
}

TEST_F(DispatchQueueTest, PathologicalStartAndStop){
  AutoRequired<Thread<1>> t1;
  AutoRequired<Thread<2>> t2;
  AutoRequired<Thread<3>> t3;
  AutoRequired<Thread<4>> t4;
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  // We don't need a strong guarantee that these threads exit in a timely fashion, just that they
  // do so _eventually_.
  t1->Stop(true);
  ASSERT_TRUE(t1->WaitFor(std::chrono::seconds(10)));
  t2->Stop(true);
  ASSERT_TRUE(t2->WaitFor(std::chrono::seconds(10)));
  t3->Stop(true);
  ASSERT_TRUE(t3->WaitFor(std::chrono::seconds(10)));
  t4->Stop(true);
  ASSERT_TRUE(t4->WaitFor(std::chrono::seconds(10)));
}

TEST_F(DispatchQueueTest, Waypoint) {
  AutoCurrentContext()->Initiate();
  AutoRequired<CoreThread> ct;

  // Barrier on an empty dispatch queue should return right away:
  ASSERT_TRUE(ct->Barrier(std::chrono::seconds(5))) << "Barrier on an empty dispatch queue did not return immediately as expected";

  // Attach a lambda that will wait until the promise is complete:
  std::promise<bool> barrier;
  *ct += [&] {
    auto f = barrier.get_future();
    f.wait();
  };
  ASSERT_FALSE(ct->Barrier(std::chrono::milliseconds(1))) << "Barrier returned even though a dispatcher was not complete";
  barrier.set_value(true);

  // Now we should be able to complete:
  ASSERT_TRUE(ct->Barrier(std::chrono::seconds(5))) << "Barrier did not return even though a dispatcher should have completed";
}
