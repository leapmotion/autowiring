// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"
#include "TestFixtures/SimpleObject.hpp"

class SnoopTest:
  public testing::Test
{};

class UpBroadcastListener {
public:
  virtual void SimpleCall(void) {}
};

class SnoopTestBase:
  public virtual UpBroadcastListener,
  public ContextMember
{
public:
  bool m_simpleCall = false;
  int m_callCount = 0;

  void SimpleCall(void) override {
    m_simpleCall = true;
    m_callCount++;
  }
};

class ChildMember:
  public SnoopTestBase
{};

class ParentMember:
  public SnoopTestBase
{};

class SiblingMember:
public SnoopTestBase
{};

class IgnoredParentMember:
  public SnoopTestBase
{};

class SimpleEvent {
public:
  virtual void ZeroArgs(void) {}
};

class RemovesSelf:
  public virtual SimpleEvent,
  public ContextMember
{
  virtual void ZeroArgs(void){
    counter++;
    AutoCurrentContext ctxt;
    ctxt->RemoveSnooper(GetSelf<RemovesSelf>());
  }
public:
  int counter = 0;
};

TEST_F(SnoopTest, SimplePackets) {
  AutoCurrentContext()->Initiate();

  AutoCreateContext Pipeline;
  AutoCreateContext Tracking;
  Pipeline->Initiate();
  Tracking->Initiate();

  // Add filter to tracking
  AutoRequired<FilterA> filter(Tracking);
  AutoRequired<FilterF> detachedFilter(Tracking);
  Pipeline->AddSnooper(filter);
  ASSERT_FALSE(!!filter->m_called) << "Filter called prematurely";
  ASSERT_FALSE(detachedFilter->m_called) << "Filter called prematurely";

  // Add factory to pipeline
  AutoRequired<AutoPacketFactory> factory(Pipeline);
  auto packet = factory->NewPacket();

  packet->Decorate(Decoration<0>());
  ASSERT_FALSE(!!filter->m_called) << "Filter called prematurely";

  // Now compleletly satisfy filter. Should snoop across contexts
  packet->Decorate(Decoration<1>());
  ASSERT_TRUE(!!filter->m_called) << "A snooper did not receive an AutoPacket originating in a snooped context";
  ASSERT_FALSE(detachedFilter->m_called) << "Received a packet from a different context";

  //reset
  filter->m_called = false;
  Pipeline->RemoveSnooper(filter);
  auto packet2 = factory->NewPacket();
  packet2->Decorate(Decoration<0>());
  packet2->Decorate(Decoration<1>());
  ASSERT_FALSE(!!filter->m_called) << "Unsnoop didn't work";
}

TEST_F(SnoopTest, CanSnoopAutowired) {
  AutoCurrentContext ctxt;
  ctxt->Inject<SimpleObject>();

  // Now autowire what we injected and verify we can snoop this directly
  Autowired<SimpleObject> so;
  ctxt->AddSnooper(so);
}
