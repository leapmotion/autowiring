// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"
#include <autowiring/autowiring.h>
#include <autowiring/AutoSelfUpdate.h>
#include <autowiring/auto_prev.h>

class AutoFilterSequencing:
  public testing::Test
{
public:
  AutoFilterSequencing(void) {
    AutoCurrentContext()->Initiate();
  }
};

class FilterFirst {
public:
  FilterFirst(void) :
    m_magic(0xDEADBEEF),
    m_called(0)
  {}

  const int m_magic;
  int m_called;

  void AutoFilter(AutoPacket& pkt) {
    ASSERT_EQ(0xDEADBEEF, m_magic) << "Magic value was corrupted, pointer was not adjusted to the correct offset";
    ++m_called;
    pkt.Decorate(Decoration<0>());
  }
};

class FilterFirstValidateInheritance:
  public FilterFirst
{};

static_assert(
  std::is_same<
  FilterFirst,
  Decompose<decltype(&FilterFirstValidateInheritance::AutoFilter)>::type
  >::value,
  "Decomposed type did not correctly name the implementing type of an inherited method"
  );

TEST_F(AutoFilterSequencing, VerifyFirstLastCalls) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<FilterFirst> first;

  {
    auto pkt = factory->NewPacket();
    ASSERT_EQ(1, first->m_called) << "First-call filter was not applied";
  }
  ASSERT_EQ(1, first->m_called) << "First-call filter was applied as final call";
}

class FilterOutDeferred:
  public CoreThread
{
public:
  Deferred AutoFilter(auto_out<Decoration<0>> out) {
    //Default constructor sets i == 0
    return Deferred(this);
  }
};

TEST_F(AutoFilterSequencing, SuccessorAliasRules) {
  AutoRequired<AutoPacketFactory> factory;
  auto packet1 = factory->NewPacket();
  auto packet2 = packet1->Successor();
  ASSERT_TRUE(!!packet2) << "Returned successor packet was null";

  auto expected = factory->NewPacket();

  ASSERT_EQ(expected, packet2) << "Expected that the successor packet would match the next packet returned by the factory";
}

TEST_F(AutoFilterSequencing, SuccessorHoldViolationCheck) {
  AutoRequired<AutoPacketFactory> factory;
  auto packet1 = factory->NewPacket();
  auto packet2 = packet1->Successor();

  ASSERT_TRUE(packet1.unique()) << "Expected that the first issued packet shared pointer not to be aliased anywhere";

  packet1.reset();
  factory->NewPacket();

  ASSERT_TRUE(packet2.unique()) << "Expected that a successor packet would be unique when the principal was destroyed";
}

TEST_F(AutoFilterSequencing, PacketReverseSuccessor) {
  AutoRequired<AutoPacketFactory> factory;

  auto packet1 = factory->NewPacket();
  auto packet2 = factory->NewPacket();

  ASSERT_EQ(packet2, packet1->Successor()) << "Successor packet obtained after generation from the factory did not match as expected";
}

TEST_F(AutoFilterSequencing, ManySuccessors) {
  AutoRequired<AutoPacketFactory> factory;
  {
    auto packetA = factory->NewPacket();
    auto packet5 = packetA->Successor()->Successor()->Successor()->Successor();

    factory->NewPacket();
    factory->NewPacket();
    factory->NewPacket();
    auto packetE = factory->NewPacket();
  
    ASSERT_EQ(packet5, packetE) << "Successor packet obtained after generation from the factory did not match as expected";
  }
  
  AutoRequired<FilterFirst> first;
  {
    auto packetA = factory->NewPacket();
    packetA->Successor()->Successor()->Successor()->Successor();
    ASSERT_EQ(1, first->m_called) << "AutoFilter triggered from successor";

    factory->NewPacket();
    ASSERT_EQ(2, first->m_called) << "AutoFilter not triggered from new packet";
    factory->NewPacket();
    ASSERT_EQ(3, first->m_called) << "AutoFilter not triggered from new packet";
    factory->NewPacket();
    ASSERT_EQ(4, first->m_called) << "AutoFilter not triggered from new packet";
    factory->NewPacket();
    ASSERT_EQ(5, first->m_called) << "AutoFilter not triggered from new packet";
    factory->NewPacket();
    ASSERT_EQ(6, first->m_called) << "AutoFilter not triggered from new packet";
  }
}

class PrevFilter {
public:
  PrevFilter(void):
    m_prev_value(-1),
    m_called(0),
    m_num_empty_prev(0)
  {}
  
  int m_prev_value;
  int m_called;
  int m_num_empty_prev;
  
  void AutoFilter(int current, auto_prev<int> prev) {
    ++m_called;
    if (prev) {
      EXPECT_EQ(m_prev_value, *prev) << "auto_prev isn't set to the previous value";
    } else {
      m_num_empty_prev++;
    }
    m_prev_value = current;
  }
};

TEST_F(AutoFilterSequencing, SimplePrev) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<PrevFilter> filter;
  
  for (int i=10; i<20; ++i) {
    auto packet = factory->NewPacket();
    packet->Decorate(i);
  }
  
  ASSERT_EQ(10, filter->m_called) << "AutoFilter not called";
  ASSERT_EQ(1, filter->m_num_empty_prev) << "Prev should only be null for the first call";
}

TEST_F(AutoFilterSequencing, UnsatisfiedPrev) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<PrevFilter> filter;

  {
    auto packet = factory->NewPacket();
    packet->Decorate(42);

    // Don't decorate this packet
    auto emptyPacket = factory->NewPacket();
  }

  ASSERT_EQ(1, filter->m_called);

  {
    auto packet = factory->NewPacket();

    // Prev from previous packet should have been marked unsatisfiable
    packet->Decorate(1337);
    ASSERT_EQ(2, filter->m_called) << "auto_prev<int> not marked unsatisfiable on this packet";
    ASSERT_EQ(2, filter->m_num_empty_prev);
  }
}

struct OnlyPrev {
  OnlyPrev():
    m_called(0)
  {}
  
  void AutoFilter(auto_prev<int> p) {
    ++m_called;
  }
  
  int m_called;
};

TEST_F(AutoFilterSequencing, OnlyPrev) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<OnlyPrev> filter;
  
  for (int i=10; i<20; ++i) {
    auto packet = factory->NewPacket();
    packet->Decorate(i);
  }
  
  ASSERT_EQ(10, filter->m_called) << "Filter not called for every packet decoration";
}

TEST_F(AutoFilterSequencing, FirstPrev) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<OnlyPrev> filter;
  
  int prev2 = 0;
  *factory += [&prev2] (auto_prev<int, 2> prev){
    prev2++;
  };

  auto packet = factory->NewPacket();

  ASSERT_EQ(1, filter->m_called) << "First packet doesn't have auto_prev value satisfied";
  ASSERT_EQ(1, prev2) << "Filter called incorrect number of times";
  
  auto packet2 = factory->NewPacket();
  ASSERT_EQ(2, prev2) << "Filter called incorrect number of times";
}

class DeferredPrev:
  public CoreThread
{
public:
  DeferredPrev(void):
    m_called(0)
  {}
  
  Deferred AutoFilter(auto_prev<int> prev) {
    ++m_called;
    return Deferred(this);
  }
  
  int m_called;
};

TEST_F(AutoFilterSequencing, DeferredPrev) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<DeferredPrev> def;
  
  factory->NewPacket()->Decorate(42);
  factory->NewPacket();
  
  *def += [def] {
    ASSERT_EQ(2, def->m_called);
  };
  
  AutoCurrentContext()->SignalShutdown(true);
}

class PrevPrevFilter {
public:
  PrevPrevFilter(void):
    m_prev_prev_value(-1),
    m_prev_value(-1),
    m_called(0),
    m_num_empty_prev(0)
  {}

  int m_prev_prev_value;
  int m_prev_value;
  int m_called;
  int m_num_empty_prev;

  void AutoFilter(int current, auto_prev<int, 2> prev) {
    ++m_called;
    if (prev) {
      EXPECT_EQ(m_prev_prev_value, *prev) << "auto_prev isn't set to the previous value";
    } else {
      m_num_empty_prev++;
    }
    m_prev_prev_value = m_prev_value;
    m_prev_value = current;
  }
};

TEST_F(AutoFilterSequencing, DoublePrev) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<PrevPrevFilter> filter;

  for (int i=10; i<20; ++i) {
    auto packet = factory->NewPacket();
    packet->Decorate(i);
  }

  ASSERT_EQ(10, filter->m_called) << "AutoFilter not called";
  ASSERT_EQ(2, filter->m_num_empty_prev) << "Prev should only be null for the first two calls";
}
