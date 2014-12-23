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
  void AutoFilter(Decoration<0> d0) { }
};

class APReceiver2 {
public:
  APReceiver2(void) {}
  void AutoFilter(Decoration<0> d0, Decoration<1>& d1) { }
};

class APReceiver3 {
public:
  APReceiver3(void) {}
  void AutoFilter(Decoration<0> d0, Decoration<1> d1) { }
};

class APReceiver4 {
public:
  APReceiver4(void) {}
  void AutoFilter(Decoration<0> d0, Decoration<2> d2) { }
};


TEST_F(AutoPacketGraphTest, VerifyEmptyGraphBeforeCtxtInit) {
  AutoCreateContext ctxt;
  CurrentContextPusher pshr(ctxt);
  
  AutoRequired<AutoPacketGraph> graph;
  ASSERT_TRUE(graph->GetEdgeCounts().empty())
    << "Graph did not start out empty before context initiation";
  
  ctxt->Initiate();
  
  ASSERT_TRUE(graph->GetEdgeCounts().empty())
    << "Graph did not stay empty after context initiation";
  
  ctxt->SignalShutdown();
}

TEST_F(AutoPacketGraphTest, VerifyEmptyGraphAfterCtxtInit) {
  AutoCreateContext ctxt;
  CurrentContextPusher pshr(ctxt);
  
  ctxt->Initiate();
  
  AutoRequired<AutoPacketGraph> graph;
  ASSERT_TRUE(graph->GetEdgeCounts().empty())
    << "Graph did not start out empty";
  
  ctxt->SignalShutdown();
}

TEST_F(AutoPacketGraphTest, VerifySimpleEdgeFromObjectBeforeInit) {
  AutoCreateContext ctxt;
  CurrentContextPusher pshr(ctxt);
  
  AutoRequired<AutoPacketGraph> graph;
  AutoRequired<APReceiver1> receiver1;
  
  ASSERT_TRUE(graph->GetEdgeCounts().empty())
    << "Graph should still be empty before context is initialized";
  
  ctxt->Initiate();
  
  ASSERT_EQ(1UL, graph->GetEdgeCounts().size())
    << "Graph did not detect AutoFilter from object after being initiated";
  
  ctxt->SignalShutdown();
}

TEST_F(AutoPacketGraphTest, VerifySimpleEdgeFromNewObject) {
  AutoCreateContext ctxt;
  CurrentContextPusher pshr(ctxt);
  
  AutoRequired<AutoPacketGraph> graph;
  
  ctxt->Initiate();
  
  ASSERT_TRUE(graph->GetEdgeCounts().empty())
    << "Graph should still be empty before context is initialized";
  
  AutoRequired<APReceiver1> receiver1;
  
  ASSERT_EQ(1UL, graph->GetEdgeCounts().size())
    << "Graph did not detect AutoFilter from object after being initiated";
  
  ctxt->SignalShutdown();
}

TEST_F(AutoPacketGraphTest, VerifyLoadGraphAfterInitAndObject) {
  AutoCreateContext ctxt;
  CurrentContextPusher pshr(ctxt);
  
  AutoRequired<APReceiver1> receiver1;
  
  ctxt->Initiate();
  
  AutoRequired<AutoPacketGraph> graph;
  
  ASSERT_EQ(1UL, graph->GetEdgeCounts().size())
    << "Graph did not detect AutoFilter from object after being initiated";
  
  ctxt->SignalShutdown();
}

TEST_F(AutoPacketGraphTest, VerifySimpleInputFilter) {
  // TODO
}

TEST_F(AutoPacketGraphTest, VerifySimpleOutputFilter) {
  // TODO
}

TEST_F(AutoPacketGraphTest, VerifyPacketDecorationIncrementingCount) {
  // TODO
}

TEST_F(AutoPacketGraphTest, VerifyLoadAutoFilterSystem) {
  AutoCreateContext ctxt;
  CurrentContextPusher pshr(ctxt);
  
  AutoRequired<AutoPacketFactory> factory(ctxt);
  AutoRequired<AutoPacketGraph> graph;
  
  ctxt->Initiate();
  
  AutoRequired<APReceiver1> receiver1;
  AutoRequired<APReceiver2> receiver2;
  AutoRequired<APReceiver3> receiver3;
  AutoRequired<APReceiver4> receiver4;
  
  AutoPacketGraph::t_deliveryEdges edges = graph->GetEdgeCounts();
  
  {
    auto packet = factory->NewPacket();
    packet->Decorate(Decoration<0>(0));
    packet->Decorate(Decoration<2>(2));
  }
  
  ASSERT_EQ(7UL, edges.size())
    << "Graph could not load the edges from new objects";
  
  for (auto& itr : edges) {
    ASSERT_EQ(0UL, itr.second)
      << "Coutn should be 0 since packets have not been delivered yet";
  }
  
  ctxt->SignalShutdown();
}

