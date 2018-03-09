// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"
#include <autowiring/autowiring.h>
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
  const int m_magic = 0xDEADBEEF;
  int m_called = 0;

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

TEST_F(AutoFilterSequencing, UnsatisfiableImmediate) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();
  AutoRequired<AutoPacketFactory> factory;

  size_t oneInRunCt = 0;
  size_t twoInRunCt = 0;

  *factory += [&oneInRunCt](const Decoration<0>&, Decoration<4>&) {
    oneInRunCt++;
  };
  *factory += [&twoInRunCt](const Decoration<0>&, const Decoration<1>&, const Decoration<4>&, Decoration<40>&) {
    twoInRunCt++;
  };

  auto packet = factory->NewPacket();

  // This should cause the first filter to be run:
  packet->DecorateImmediate(Decoration<0>{});
  ASSERT_NO_THROW(packet->Get<Decoration<4>>()) << "Single argument immediate filter was not run as expected";
  ASSERT_EQ(1UL, oneInRunCt) << "Single argument filter not run the expected number of times";

  // If we didn't use DecorateImmediate before, then this would normally cause the second filter to be run
  ASSERT_EQ(0UL, twoInRunCt) << "A zero-argument immediate filter was incorrectly run";
  ASSERT_NO_THROW(packet->DecorateImmediate(Decoration<1>{}));
}
