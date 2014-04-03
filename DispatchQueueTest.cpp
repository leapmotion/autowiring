// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "DispatchQueueTest.h"
#include "DispatchQueue.h"
#include "CoreThread.h"

using namespace std;

class EventMaker:
  public CoreThread
{
public:
  EventMaker():
    CoreThread("EventMaker")
  {
    AcceptDispatchDelivery();
  }
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
  m_create->InitiateCoreThreads();
  t1->DelayUntilCanAccept();

  // We don't need a strong guarantee that these threads exit in a timely fashion, just that they
  // do so _eventually_.
  t1->Stop(true);
  ASSERT_TRUE(t1->WaitFor(boost::chrono::seconds(10)));
  t2->Stop(true);
  ASSERT_TRUE(t2->WaitFor(boost::chrono::seconds(10)));
  t3->Stop(true);
  ASSERT_TRUE(t3->WaitFor(boost::chrono::seconds(10)));
  t4->Stop(true);
  ASSERT_TRUE(t4->WaitFor(boost::chrono::seconds(10)));
}


