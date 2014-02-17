// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "DispatchQueueTest.h"
#include "DispatchQueue.h"

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


class ThreadBase:
  public CoreThread
{
public:
  ThreadBase(){
    AcceptDispatchDelivery();
  }
};

template<int N>
class Thread:
  public ThreadBase
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

TEST_F(DispatchQueueTest, SingleThreadCommits){
  int count = 0;
  {
    Commision com = *this += [&count] () {
      count+=1;
    };
    
    DispatchAllEvents();
  
    *this += [&count] () {
      count+=2;
    };
    
    DispatchAllEvents();
  
    Commision com2 = *this += [&count] {
      count+=4;
    };
    
    DispatchAllEvents();
    
    EXPECT_EQ(0, count);
    
    com.Commit();
    
    DispatchAllEvents();
    
    EXPECT_EQ(3, count);
  }
  
  DispatchAllEvents();
  
  EXPECT_EQ(7, count);
}

#if _WIN32
TEST_F(DispatchQueueTest, DISABLED_MultiThreadCommits)
#else
TEST_F(DispatchQueueTest, MultiThreadCommits)
#endif
{
  AutoRequired<EventMaker> eventMaker;
  int count = 0;
  
  boost::mutex mut;
  boost::unique_lock<boost::mutex> lk(mut);
  boost::condition_variable cond;
  
  m_create->InitiateCoreThreads();
  
  *eventMaker += [&count, &cond, &mut] () {
    boost::unique_lock<boost::mutex> lk(mut);
    count += 1;
    cond.notify_all();
  };
  
  Commision com1 = *eventMaker += [&count, &cond, &mut] () {
    boost::unique_lock<boost::mutex> lk(mut);
    count += 2;
  };
  
  *eventMaker += [&count, &cond, &mut] () {
    boost::unique_lock<boost::mutex> lk(mut);
    count += 4;
    cond.notify_all();
  };
  
  EXPECT_FALSE( boost::cv_status::timeout == cond.wait_for(lk,boost::chrono::microseconds(500)) )
  << "Waited more than 500ms on condition variable";
  
  EXPECT_EQ(1, count);
  
  com1.Commit();
  
  EXPECT_FALSE( boost::cv_status::timeout == cond.wait_for(lk,boost::chrono::microseconds(500)) )
  << "Waited more than 500ms on condition variable";
  
  EXPECT_EQ(7, count);
}

TEST_F(DispatchQueueTest, PathologicalStartAndStop){
  AutoRequired<Thread<1>> t1;
  AutoRequired<Thread<2>> t2;
  AutoRequired<Thread<3>> t3;
  AutoRequired<Thread<4>> t4;
  m_create->InitiateCoreThreads();
  t1->Stop(true);
  EXPECT_TRUE(t1->WaitFor(boost::chrono::microseconds(200)));
  t2->Stop(true);
  EXPECT_TRUE(t2->WaitFor(boost::chrono::microseconds(200)));
  t3->Stop(true);
  EXPECT_TRUE(t3->WaitFor(boost::chrono::microseconds(200)));
  t4->Stop(true);
  EXPECT_TRUE(t4->WaitFor(boost::chrono::microseconds(200)));
}


