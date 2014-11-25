// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"
#include <autowiring/autowiring.h>
#include <autowiring/AutoMerge.h>
#include <autowiring/AutoStile.h>
#include <autowiring/AutoSelfUpdate.h>
#include <autowiring/DeclareElseFilter.h>

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

class FilterLast {
public:
  int m_called;

  FilterLast() : m_called(0) {};

  void AutoFilter(const AutoPacket& pkt) {
    ++m_called;
    ASSERT_TRUE(pkt.Has<Decoration<0>>()) << "Missing FilterFirst Decoration<0>";
  }
};

class FilterLastD0 {
public:
  int m_called;

  FilterLastD0() : m_called(0) {};

  void AutoFilter(const AutoPacket& pkt, const Decoration<0>& dec) {
    ++m_called;
    ASSERT_EQ(0, dec.i) << "Incorrect decoration value";
  }
};

class FilterLastD1 {
public:
  int m_called;

  FilterLastD1() : m_called(0) {};

  void AutoFilter(const AutoPacket& pkt, const Decoration<1>& dec) {
    ++m_called;
    FAIL() << "Final-Call to AutoFilter with unsatisfied type";
  }
};

TEST_F(AutoFilterSequencing, VerifyFirstLastCalls) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<FilterFirst> first;
  AutoRequired<FilterLast> last;
  AutoRequired<FilterLastD0> lastD0;
  AutoRequired<FilterLastD1> lastD1;

  {
    auto pkt = factory->NewPacket();
    ASSERT_EQ(1, first->m_called) << "First-call filter was not applied";
    ASSERT_EQ(0, last->m_called) << "Last-call filter was called early";
    ASSERT_EQ(0, lastD0->m_called) << "Last-call filter was not applied";
  }
  ASSERT_EQ(1, first->m_called) << "First-call filter was applied as final call";
  ASSERT_EQ(1, lastD0->m_called) << "Last-call filter was not applied";
}

class LogicFilter {
protected:
  typedef BasedAutoFilter<LogicFilter, void, const Decoration<1>&> t_Next;
  std::shared_ptr<MicroElseFilter<LogicFilter>> m_MicroElseFilter;
  std::shared_ptr<t_Next> m_BasedNextFilter;
  std::shared_ptr<MicroElseFilter<t_Next>> m_MicroNextElseFilter;

public:
  int m_calledAuto;
  int m_calledElse;
  int m_calledNext;
  int m_calledNextElse;

  LogicFilter() :
    m_calledAuto(0),
    m_calledElse(0),
    m_calledNext(0),
    m_calledNextElse(0)
  {
    m_MicroElseFilter = DeclareElseFilter(this, &LogicFilter::ElseFilter);
    m_BasedNextFilter = DeclareAutoFilter(this, &LogicFilter::NextFilter);
    m_MicroNextElseFilter = DeclareElseFilter<LogicFilter, t_Next>(this, &LogicFilter::NextElseFilter);
    Reset();
  }

  /// Normal AutoFilter call, implemented by MicroAutoFilter
  void AutoFilter(const Decoration<0>& deco) {
    ++m_calledAuto;
  }

  /// Called when AutoFilter is not
  void ElseFilter(const AutoPacket& packet) {
    ++m_calledElse;
    ASSERT_FALSE(packet.Has<Decoration<0>>()) << "AutoFilter should not have been called";
  }

  /// Declared AutoFilter call, implemented by MicroAutoFilter
  void NextFilter(const Decoration<1>& deco) {
    ++m_calledNext;
  }

  /// Declared AutoFilter call, implemented by MicroAutoFilter
  void NextElseFilter(const AutoPacket& packet) {
    ++m_calledNextElse;
    ASSERT_FALSE(packet.Has<Decoration<1>>()) << "NextFilter should not have been called";
  }

  void Reset() {
    m_calledAuto = 0;
    m_calledElse = 0;
    m_calledNext = 0;
    m_calledNextElse = 0;
  }
};

TEST_F(AutoFilterSequencing, TestLogicFilter) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<LogicFilter> logic;

  // Issue & return a packet without decorating
  {
    auto pkt = factory->NewPacket();
    ASSERT_EQ(0, logic->m_calledAuto) << "Called AutoFilter without Decoration<0>";
    ASSERT_EQ(0, logic->m_calledElse) << "Called ElseFilter before packet final-calls";
    ASSERT_EQ(0, logic->m_calledNext) << "Called NextFilter without Decoration<0>";
    ASSERT_EQ(0, logic->m_calledNextElse) << "Called NextElseFilter before packet final-calls";
  }
  ASSERT_EQ(0, logic->m_calledAuto) << "Called AutoFilter without Decoration<0>";
  ASSERT_EQ(1, logic->m_calledElse) << "Failed to call ElseFilter in packet final-calls";
  ASSERT_EQ(0, logic->m_calledNext) << "Called NextFilter without Decoration<0>";
  ASSERT_EQ(1, logic->m_calledNextElse) << "Failed to call NextElseFilter in packet final-calls";
  logic->Reset();

  // Issue & decoration a packet
  {
    auto pkt = factory->NewPacket();
    pkt->Decorate(Decoration<0>());
    ASSERT_EQ(1, logic->m_calledAuto) << "Failed to call AutoFilter with Decoration<0>";
    ASSERT_EQ(0, logic->m_calledElse) << "Called ElseFilter before packet final-calls";
    ASSERT_EQ(0, logic->m_calledNext) << "Called NextFilter without Decoration<0>";
    ASSERT_EQ(0, logic->m_calledNextElse) << "Called NextElseFilter before packet final-calls";

    pkt->Decorate(Decoration<1>());
    ASSERT_EQ(1, logic->m_calledAuto) << "Multiple calls to AutoFilter with Decoration<0>";
    ASSERT_EQ(0, logic->m_calledElse) << "Called ElseFilter before packet final-calls";
    ASSERT_EQ(1, logic->m_calledNext) << "Failed to call NextFilter with Decoration<1>";
    ASSERT_EQ(0, logic->m_calledNextElse) << "Called NextElseFilter before packet final-calls";
  }
  ASSERT_EQ(1, logic->m_calledAuto) << "Multiple calls to AutoFilter with Decoration<0>";
  ASSERT_EQ(0, logic->m_calledElse) << "Called ElseFilter in packet final-calls";
  ASSERT_EQ(1, logic->m_calledNext) << "Multiple calls to NextFilter with Decoration<1>";
  ASSERT_EQ(0, logic->m_calledNextElse) << "Called NextElseFilter in packet final-calls";
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

TEST_F(AutoFilterSequencing, VerifyAutoOutDeferred) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<FilterOutDeferred> out;
  AutoRequired<FilterLast> last;
  {
    std::shared_ptr<AutoPacket> packet = factory->NewPacket();
    // Creation of packet triggers Deferred AutoFilter call
    // Destruction of packet trigerrs FilterLast, which tests
    // for the existence of Decoration<0> on the packet
  }
  out->Stop(true); // Run-down all queued calls (out)
  out->Wait(); // Wait for calls to compelte (last)

  ASSERT_EQ(last->m_called, 1) << "FilterLast was not called";
}

class FilterFinalFail1 {
public:
  void AutoFilter(optional_ptr<Decoration<0>>, auto_out<Decoration<1>>) {}
};

class FilterFinalFail2 {
public:
  void AutoFilter(const AutoPacket&, auto_out<Decoration<1>>) {}
};

TEST_F(AutoFilterSequencing, DISABLED_VerifyFinalImmutability) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<FilterFinalFail1> fail1;
  ASSERT_THROW(factory->NewPacket(), std::runtime_error) << "Output holds shared_ptr to packet, which is invalid in Finalize";

  // PROBLEM: Exception is thrown correctly, but is not caught by test.
  AutoRequired<FilterFinalFail2> fail2;
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>());
  ASSERT_THROW(factory->NewPacket(), std::runtime_error) << "Output holds shared_ptr to packet, which is invalid in Finalize";
}
