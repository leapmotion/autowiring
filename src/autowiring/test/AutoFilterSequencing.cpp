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
