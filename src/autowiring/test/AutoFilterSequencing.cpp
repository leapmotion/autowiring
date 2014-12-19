// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"
#include <autowiring/autowiring.h>
#include <autowiring/AutoSelfUpdate.h>

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
