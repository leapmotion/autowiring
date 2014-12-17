// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"
#include <autowiring/AutoPacketFactory.h>
#include <autowiring/AutoPacketGraph.h>
#include CHRONO_HEADER
#include THREAD_HEADER

class AutoPacketGraphTest:
  public testing::Test
{};

class APReceiver1 {
public:
  APReceiver1(void) {}
  
  void AutoFilter(Decoration<0> d0) {
    m_int0 = d0.i;
  }
  
  int m_int0;
};

class APReceiver2 {
public:
  APReceiver2(void) {}
  
  void AutoFilter(Decoration<0> d0, Decoration<1>& d1) {
    m_int0 = d0.i;
    
    d1.i = m_int1;
  }
  
  int m_int0;
  int m_int1;
};

class APReceiver3 {
public:
  APReceiver3(void) {}
  
  void AutoFilter(Decoration<0> d0, Decoration<1> d1) {
    m_int0 = d0.i;
    m_int1 = d1.i;
  }
  
  int m_int0;
  int m_int1;
};

class APReceiver4 {
public:
  APReceiver4(void) {}
  
  void AutoFilter(Decoration<0> d0, Decoration<2> d2) {
    m_int0 = d0.i;
    m_int2 = d2.i;
  }
  
  int m_int0;
  int m_int2;
};

TEST_F(AutoPacketGraphTest, SimpleAutoGraph) {
  AutoCreateContext ctxt;
  CurrentContextPusher pshr(ctxt);
  
  AutoRequired<AutoPacketFactory> factory(ctxt);
  AutoRequired<AutoPacketGraph> graph;
  
  AutoRequired<APReceiver1> receiver1;
  AutoRequired<APReceiver2> receiver2;
  AutoRequired<APReceiver3> receiver3;
  AutoRequired<APReceiver4> receiver4;
  
  ctxt->Initiate();
  
  int int0 = 12;
  int int1 = 34;
  int int2 = 56;
  
  receiver2->m_int1 = int1;
  
  {
    // decorate 1
    auto packet = factory->NewPacket();
    packet->Decorate(Decoration<0>(int0));

    // TODO: add some real test cases
//    ASSERT_EQ(int0, receiver1->m_int0);
//    
//    ASSERT_EQ(int0, receiver2->m_int0);
//    
//    ASSERT_EQ(int0, receiver3->m_int0);
//    ASSERT_EQ(int1, receiver3->m_int1);
    
    // decorate 2
    packet->Decorate(Decoration<2>(int2));

    // TODO: add some real test cases
//    ASSERT_EQ(int0, receiver4->m_int0);
//    ASSERT_EQ(int2, receiver4->m_int2);
  }
  
  graph->WriteGV("/Users/jnguyen/Desktop/graph.gv");
  
  // Shutdown our context, and rundown our factory
  ctxt->SignalShutdown();
  factory->Wait();
  
}