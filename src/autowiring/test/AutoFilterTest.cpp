// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"
#include <autowiring/AutoPacket.h>
#include <autowiring/AutoPacketFactory.h>
#include <autowiring/Deferred.h>
#include <autowiring/optional_ptr.h>
#include <autowiring/NewAutoFilter.h>
#include <autowiring/DeclareAutoFilter.h>
#include <autowiring/DeclareElseFilter.h>
#include <autowiring/AutoSelfUpdate.h>
#include <autowiring/AutoTimeStamp.h>
#include <autowiring/SatCounter.h>
#include <autowiring/AutoMerge.h>
#include <autowiring/AutoStile.h>
#include THREAD_HEADER

class AutoFilterTest:
  public testing::Test
{
public:
  AutoFilterTest(void) {
    // All decorator tests must run from an initiated context
    AutoCurrentContext()->Initiate();
  }
};

TEST_F(AutoFilterTest, VerifyDescendentAwareness) {
  // Create a packet while the factory has no subscribers:
  AutoRequired<AutoPacketFactory> parentFactory;
  std::shared_ptr<AutoPacket> firstPacket = parentFactory->NewPacket();

  // Verify subscription-free status:
  EXPECT_FALSE(firstPacket->HasSubscribers(typeid(Decoration<0>))) << "Subscription exists where one should not have existed";

  std::shared_ptr<AutoPacket> strongPacket;
  std::weak_ptr<AutoPacket> weakPacket;
  std::weak_ptr<FilterA> filterChecker;

  // Create a subcontext
  {
    AutoCreateContext subContext;
    {
      CurrentContextPusher pusher(subContext);

      //add a filter in the subcontext
      AutoRequired<FilterA> subFilter;
      filterChecker = subFilter;
    }
    EXPECT_FALSE(firstPacket->HasSubscribers(typeid(Decoration<0>))) << "Subscription was incorrectly, retroactively added to a packet";

    //Create a packet where a subscriber exists only in a subcontext
    strongPacket = parentFactory->NewPacket();
    std::shared_ptr<AutoPacket> holdPacket = parentFactory->NewPacket();
    weakPacket = holdPacket;
    EXPECT_TRUE(strongPacket->HasSubscribers(typeid(Decoration<0>))) << "Packet lacked expected subscription from subcontext";
    EXPECT_TRUE(weakPacket.lock()->HasSubscribers(typeid(Decoration<0>))) << "Packet lacked expected subscription from subcontext";
  }
  EXPECT_TRUE(weakPacket.expired()) << "Packet was not destroyed when it's subscribers were removed";
  EXPECT_FALSE(filterChecker.expired()) << "Packet keeping subcontext member alive";

  // Verify the second packet will no longer have subscriptions  -
  // normally removing a subscriber would mean the packet still has the subscriber, but
  // in this case, the subscriber was actually destroyed so the packet has lost a subscriber.
  EXPECT_TRUE(strongPacket->HasSubscribers(typeid(Decoration<0>))) << "Missing subscriber from destroyed subcontext";

  // Call the subscriber... this will either succeed or segfault.
  strongPacket->Decorate(Decoration<0>());
  strongPacket->Decorate(Decoration<1>());
  EXPECT_TRUE(strongPacket->HasSubscribers(typeid(Decoration<0>))) << "Calling a subscriber should not remove it";
  {
    std::shared_ptr<FilterA> holdFilter = filterChecker.lock();
    ASSERT_EQ(1, holdFilter->m_called) << "Subcontext filter was not called";
  }

  // Create a packet after the subcontext has been destroyed
  auto lastPacket = parentFactory->NewPacket();
  EXPECT_FALSE(lastPacket->HasSubscribers(typeid(Decoration<0>))) << "Subscription was incorrectly, retroactively added to a packet";

  // Verify that strongPacket was responsible for keeping subFilter alive
  strongPacket.reset();
  EXPECT_TRUE(filterChecker.expired()) << "Subscriber from destroyed subcontext didn't expire after packet was reset.";
}

TEST_F(AutoFilterTest, VerifySimpleFilter) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<FilterA> filterA;

  // Verify that the subscriber has been properly detected:
  bool bFound = false;
  std::vector<AutoFilterDescriptor> descs;
  factory->AppendAutoFiltersTo(descs);
  for(const auto& cur : descs)
    if(cur.GetAutoFilter() == filterA) {
      bFound = true;
      break;
    }

  ASSERT_TRUE(bFound) << "Failed to find an added subscriber ";

  // Obtain a packet from the factory:
  auto packet = factory->NewPacket();

  // Decorate with one instance:
  packet->Decorate(Decoration<0>());

  // Verify that no hit takes place with inadequate decoration:
  EXPECT_FALSE(filterA->m_called) << "Filter called prematurely with insufficient decoration";

  // Now decorate with the other requirement of the filter:
  packet->Decorate(Decoration<1>());

  // A hit should have taken place at this point:
  EXPECT_LT(0, filterA->m_called) << "Filter was not called even though it was fully satisfied";
}

template<int N>
class ChildDecoration : Decoration<N> {};

TEST_F(AutoFilterTest, VerifyTypeUsage) {
  AutoRequired<FilterA> filterA;
  AutoRequired<AutoPacketFactory> factory;

  // EXPECT: No attempt is made to cast decorations to parent types.
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>()); // Fulfills first requirement
  ASSERT_EQ(0, filterA->m_called) << "AutoFilter called with incomplete arguments";
  packet->Decorate(ChildDecoration<1>()); // Does not fulfill second requirement
  ASSERT_EQ(0, filterA->m_called) << "AutoFilter using derived type";
  EXPECT_NO_THROW(packet->Decorate(Decoration<1>(2))) << "Decoration with parent type conflicts with derived type";
  ASSERT_EQ(1, filterA->m_called) << "AutoFilter was not called when all arguments were available";
  ASSERT_EQ(2, filterA->m_one.i) << "AutoFilter was called using derived type instead of parent";
}

class FilterFirst {
public:
  int m_called;

  FilterFirst() : m_called(0) {};

  void AutoFilter(AutoPacket& pkt) {
    ++m_called;
    pkt.Decorate(Decoration<0>());
  }
};

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

TEST_F(AutoFilterTest, VerifyFirstLastCalls) {
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

  LogicFilter() {
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
    ASSERT_FALSE(packet.Has<Decoration<0>>()) << "AutoFilter should have been called";
  }

  /// Declared AutoFilter call, implemented by MicroAutoFilter
  void NextFilter(const Decoration<1>& deco) {
    ++m_calledNext;
  }

  /// Declared AutoFilter call, implemented by MicroAutoFilter
  void NextElseFilter(const AutoPacket& packet) {
    ++m_calledNextElse;
    ASSERT_FALSE(packet.Has<Decoration<1>>()) << "NextFilter should have been called";
  }

  void Reset() {
    m_calledAuto = 0;
    m_calledElse = 0;
    m_calledNext = 0;
    m_calledNextElse = 0;
  }
};

TEST_F(AutoFilterTest, TestLogicFilter) {
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

class FilterOut {
public:
  void AutoFilter(auto_out<Decoration<0>> out) {
    out->i = 1;
  }
};

TEST_F(AutoFilterTest, VerifyAutoOut) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<FilterOut> out;
  std::shared_ptr<AutoPacket> packet = factory->NewPacket();
  const Decoration<0>* result0 = nullptr;
  ASSERT_TRUE(packet->Get(result0)) << "Output missing";
  ASSERT_EQ(result0->i, 1) << "Output incorrect";
}

class FilterOutDeferred:
  public CoreThread
{
public:
  Deferred AutoFilter(auto_out<Decoration<0>> out) {
    out.make(); //Default constructor sets i == 0
    return Deferred(this);
  }
};

TEST_F(AutoFilterTest, VerifyAutoOutDeferred) {
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

TEST_F(AutoFilterTest, DISABLED_VerifyFinalImmutability) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<FilterFinalFail1> fail1;
  ASSERT_THROW(factory->NewPacket(), std::runtime_error) << "Output holds shared_ptr to packet, which is invalid in Finalize";

  // PROBLEM: Exception is thrown correctly, but is not caught by test.
  AutoRequired<FilterFinalFail2> fail2;
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>());
  ASSERT_THROW(factory->NewPacket(), std::runtime_error) << "Output holds shared_ptr to packet, which is invalid in Finalize";
}

TEST_F(AutoFilterTest, VerifyOptionalFilter) {
  AutoRequired<AutoPacketFactory> factory;

  AutoRequired<FilterGen<Decoration<0>, optional_ptr<Decoration<1>>>> fgA;
  AutoRequired<FilterGen<optional_ptr<Decoration<1>>, Decoration<0>>> fgB;
  AutoRequired<FilterGen<optional_ptr<Decoration<0>>, Decoration<1>>> fgC;
  AutoRequired<FilterGen<Decoration<0>, optional_ptr<Decoration<1>>, optional_ptr<Decoration<2>>>> fgD;

  //Test resolution through parameter satisfaction
  {
    auto packet = factory->NewPacket();
    packet->Decorate(Decoration<0>());

    ASSERT_TRUE(fgA->m_called == 0) << "An AutoFilter was called " << fgA->m_called << " times when an optional input was unresolved";
    ASSERT_TRUE(fgB->m_called == 0) << "An AutoFilter was called " << fgB->m_called << " times when an optional input was unresolved";
    ASSERT_TRUE(fgC->m_called == 0) << "An AutoFilter was called " << fgC->m_called << " times when a required input was not available";

    packet->Decorate(Decoration<1>());

    ASSERT_TRUE(fgA->m_called == 1) << "An AutoFilter was called " << fgA->m_called << " times when all inputs were simultaneously available";
    ASSERT_TRUE(fgB->m_called == 1) << "An AutoFilter was called " << fgB->m_called << " times when all inputs were simultaneously available";
    ASSERT_TRUE(fgC->m_called == 1) << "An AutoFilter was called " << fgC->m_called << " times when all inputs were simultaneously available";
    ASSERT_TRUE(fgD->m_called == 0) << "An AutoFilter was called " << fgD->m_called << " times when an optional input was unresolved";
  }

  fgA->m_called = 0;
  fgB->m_called = 0;
  fgC->m_called = 0;
  fgD->m_called = 0;

  //Test resultion through packet destruction
  {
    auto packet = factory->NewPacket();
    packet->Decorate(Decoration<0>());
    packet->Decorate(Decoration<2>());

    ASSERT_TRUE(fgA->m_called == 0) << "An AutoFilter was called " << fgA->m_called << " before optional arguments were resolved";
    ASSERT_TRUE(fgB->m_called == 0) << "An AutoFilter was called " << fgB->m_called << " before optional arguments were resolved";
    ASSERT_TRUE(fgC->m_called == 0) << "An AutoFilter was called " << fgC->m_called << " before optional arguments were resolved";
    ASSERT_TRUE(fgD->m_called == 0) << "An AutoFilter was called " << fgD->m_called << " before optional arguments were resolved";
  }

  ASSERT_TRUE(fgA->m_called == 1) << "An AutoFilter was called " << fgA->m_called << " times when all required inputs were available";
  ASSERT_TRUE(fgB->m_called == 1) << "An AutoFilter was called " << fgB->m_called << " times when all required inputs were available";
  ASSERT_TRUE(fgC->m_called == 0) << "An AutoFilter was called " << fgC->m_called << " times when a required input was not available";
  ASSERT_TRUE(fgD->m_called == 1) << "An AutoFilter was called " << fgD->m_called << " times when all required inputs were available";
}

TEST_F(AutoFilterTest, VerifyNoMultiDecorate) {
  AutoRequired<FilterA> filterA;
  AutoRequired<AutoPacketFactory> factory;

  // Obtain a packet and attempt redundant introduction:
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>());
  EXPECT_ANY_THROW(packet->Decorate(Decoration<0>())) << "Redundant decoration did not throw an exception as expected";

  // Verify that a call has not yet been made
  EXPECT_FALSE(filterA->m_called) << "A call made on an idempotent packet decoration";

  // Now finish saturating the filter and ensure we get a call:
  packet->Decorate(Decoration<1>());
  EXPECT_LT(0, filterA->m_called) << "Filter was not called after being fully satisfied";

  //NOTE: A typedef will throw an exception
  typedef Decoration<0> isDeco0type;
  EXPECT_ANY_THROW(packet->Decorate(isDeco0type())) << "Typedef failed to throw exception";

  //NOTE: A shared_ptr to an existing type will throw an exception
  auto sharedDeco0 = std::make_shared<Decoration<0>>();
  EXPECT_ANY_THROW(packet->Decorate(sharedDeco0)) << "Reduction of shared_ptr to base type failed";

  //NOTE: Inheritance will not throw an exception
  class ofDeco0alias: public Decoration<0> {};
  try {
    packet->Decorate(ofDeco0alias());
  } catch (...) {
    FAIL() << "Class with inheritance from existing decoration reinterpreted as child type";
  }

  // Verify that DecorateImmedaite also yields an exception
  Decoration<0> localDeco0;
  EXPECT_ANY_THROW(packet->DecorateImmediate(localDeco0)) << "Redundant immediate decoration did not throw an exception as expected";

  EXPECT_ANY_THROW(packet->DecorateImmediate(Decoration<2>(), Decoration<2>())) << "Repeated type in immediate decoration was not identified as an error";
}

TEST_F(AutoFilterTest, VerifyNoNullCheckout) {
  AutoRequired<AutoPacketFactory> factory;

  std::shared_ptr<Decoration<0>> nulldeco;
  ASSERT_FALSE(nulldeco);

  auto packet = factory->NewPacket();
  EXPECT_THROW(packet->Checkout(nulldeco), std::exception) << "Failed to catch null checkout" << std::endl;
  EXPECT_THROW(packet->Decorate(nulldeco), std::exception) << "Failed to catch null decoration" << std::endl;
}

template<int out, int in>
class FilterGather {
public:
  FilterGather():
    m_called_out(0),
    m_called_in(0),
    m_out(out),
    m_in(in)
  {}

  void AutoFilter(AutoPacket& packet) {
    ++m_called_out;
    packet.Decorate(Decoration<out>(m_out));
  }

  void AutoGather(const Decoration<in>& input) {
    ++m_called_in;
    m_in = input.i;
  }

  int m_called_out;
  int m_called_in;
  int m_out;
  int m_in;

  NewAutoFilter<decltype(&FilterGather<out,in>::AutoGather), &FilterGather<out,in>::AutoGather> FilterGather_AutoGather;
};

TEST_F(AutoFilterTest, VerifyTwoAutoFilterCalls) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<FilterGather<0,1>> zero2one;
  AutoRequired<FilterGather<1,0>> one2zero;
  zero2one->m_out = 3;
  one2zero->m_out = 4;

  //Verify that calls made on allocation from object pool do not introduce a race condition
  {
    std::shared_ptr<AutoPacket> packet = factory->NewPacket();
    ASSERT_EQ(1, zero2one->m_called_out) << "AutoFilter with AutoPacket as only argument was called " << zero2one->m_called_out << " times";
    ASSERT_EQ(1, zero2one->m_called_in) << "AutoFilter of implicitly decorated type was called " << zero2one->m_called_in << " times";
    ASSERT_EQ(4, zero2one->m_in) << "AutoFilter received incorrect input of " << zero2one->m_in;
    ASSERT_EQ(1, one2zero->m_called_out) << "AutoFilter with AutoPacket as only argument was called " << one2zero->m_called_out << " times";
    ASSERT_EQ(1, one2zero->m_called_in) << "AutoFilter of implicitly decorated type was called " << one2zero->m_called_in << " times";
    ASSERT_EQ(3, one2zero->m_in) << "AutoFilter received incorrect input of " << one2zero->m_in;
    zero2one->m_out = 5;
    one2zero->m_out = 6;
  }

  //Verify that no additional calls are made during return of packet to object pool
  ASSERT_EQ(1, zero2one->m_called_out) << "AutoFilter with AutoPacket as only argument was called " << zero2one->m_called_out << " times";
  ASSERT_EQ(1, zero2one->m_called_in) << "AutoFilter of implicitly decorated type was called " << zero2one->m_called_in << " times";
  ASSERT_EQ(4, zero2one->m_in) << "AutoFilter received incorrect input of " << zero2one->m_in;
  ASSERT_EQ(1, one2zero->m_called_out) << "AutoFilter with AutoPacket as only argument was called " << one2zero->m_called_out << " times";
  ASSERT_EQ(1, one2zero->m_called_in) << "AutoFilter of implicitly decorated type was called " << one2zero->m_called_in << " times";
  ASSERT_EQ(3, one2zero->m_in) << "AutoFilter received incorrect input of " << one2zero->m_in;
}

template<int out, int in>
class FilterGatherAutoOut {
public:
  FilterGatherAutoOut():
  m_called_out(0),
  m_called_in(0),
  m_out(out),
  m_in(in)
  {}

  void AutoFilter(auto_out<Decoration<out>> output) {
    ++m_called_out;
    output->i = m_out;
  }

  void AutoGather(const Decoration<in>& input) {
    ++m_called_in;
    m_in = input.i;
  }

  int m_called_out;
  int m_called_in;
  int m_out;
  int m_in;
  NewAutoFilter<decltype(&FilterGather<out,in>::AutoGather), &FilterGather<out,in>::AutoGather> FilterGather_AutoGather;
};

TEST_F(AutoFilterTest, VerifyTwoAutoFilterCallsAutoOut) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<FilterGatherAutoOut<0,1>> zero2one;
  AutoRequired<FilterGatherAutoOut<1,0>> one2zero;
  zero2one->m_out = 3;
  one2zero->m_out = 4;

  //Verify that calls made on allocation from object pool do not introduce a race condition
  {
    std::shared_ptr<AutoPacket> packet = factory->NewPacket();
    ASSERT_EQ(1, zero2one->m_called_out) << "AutoFilter with auto_out as only argument was called " << zero2one->m_called_out << " times";
    ASSERT_EQ(1, zero2one->m_called_in) << "AutoFilter of implicitly decorated type was called " << zero2one->m_called_in << " times";
    ASSERT_EQ(4, zero2one->m_in) << "AutoFilter received incorrect input of " << zero2one->m_in;
    ASSERT_EQ(1, one2zero->m_called_out) << "AutoFilter with auto_out as only argument was called " << one2zero->m_called_out << " times";
    ASSERT_EQ(1, one2zero->m_called_in) << "AutoFilter of implicitly decorated type was called " << one2zero->m_called_in << " times";
    ASSERT_EQ(3, one2zero->m_in) << "AutoFilter received incorrect input of " << one2zero->m_in;
    zero2one->m_out = 5;
    one2zero->m_out = 6;
  }
  //Verify that no additional calls are made during return of packet to object pool
  ASSERT_EQ(1, zero2one->m_called_out) << "AutoFilter with auto_out as only argument was called " << zero2one->m_called_out << " times";
  ASSERT_EQ(1, zero2one->m_called_in) << "AutoFilter of implicitly decorated type was called " << zero2one->m_called_in << " times";
  ASSERT_EQ(4, zero2one->m_in) << "AutoFilter received incorrect input of " << zero2one->m_in;
  ASSERT_EQ(1, one2zero->m_called_out) << "AutoFilter with auto_out as only argument was called " << one2zero->m_called_out << " times";
  ASSERT_EQ(1, one2zero->m_called_in) << "AutoFilter of implicitly decorated type was called " << one2zero->m_called_in << " times";
  ASSERT_EQ(3, one2zero->m_in) << "AutoFilter received incorrect input of " << one2zero->m_in;
}

TEST_F(AutoFilterTest, VerifyInterThreadDecoration) {
  AutoRequired<FilterB> filterB;
  AutoRequired<AutoPacketFactory> factory;
  AutoCurrentContext ctxt;

  // Obtain a packet for processing and decorate it:
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>());
  packet->Decorate(Decoration<1>());

  // Verify that the recipient has NOT yet received the message:
  EXPECT_FALSE(filterB->m_called) << "A call was made to a thread which should not have been able to process it";

  // Wake up the barrier and post a quit message:
  filterB->Continue();
  *filterB += [&filterB] { filterB->Stop(); };
  filterB->Wait();

  // Verify that the filter method has been called
  EXPECT_LT(0, filterB->m_called) << "A deferred filter method was not called as expected";
}

TEST_F(AutoFilterTest, VerifyTeardownArrangement) {
  AutoRequired<AutoPacketFactory> factory;

  std::weak_ptr<FilterA> filterAWeak;

  {
    std::shared_ptr<AutoPacket> packet;
    {
      // Create the filter and subscribe it
      auto filterA = std::make_shared<FilterA>();
      filterAWeak = filterA;
      factory->AddSubscriber(filterA);

      // Create the packet--this should lock in references to all subscribers:
      packet = factory->NewPacket();
    }

    // Verify that the subscription has not expired:
    ASSERT_FALSE(filterAWeak.expired()) << "A subscriber while it was still registered";

    {
      std::shared_ptr<FilterA> filterA = filterAWeak.lock();

      // Unsubscribe the filter:
      factory->RemoveSubscriber(MakeAutoFilterDescriptor(filterA));
    }

    // Verify that unsubscription STILL does not result in expiration:
    ASSERT_FALSE(filterAWeak.expired()) << "A subscriber expired before all packets on that subscriber were satisfied";

    //Create a new packet after having removed the only filter on it.
    auto packet2 = factory->NewPacket();
    ASSERT_FALSE(packet2->HasSubscribers(typeid(Decoration<0>))) << "A packet had subscriptions after the only subscriber was removed.";

    // Satisfy the packet:
    packet->Decorate(Decoration<0>());
    packet->Decorate(Decoration<1>());

    auto packet3 = factory->NewPacket();
    ASSERT_FALSE(packet3->HasSubscribers(typeid(Decoration<0>))) << "A packet had subscriptions after the only subscriber was removed.";
  }

  // Filter should be expired now:
  ASSERT_TRUE(filterAWeak.expired()) << "Subscriber was still left outstanding even though all references should be gone";
}

TEST_F(AutoFilterTest, VerifyCheckout) {
  AutoRequired<FilterA> filterA;
  AutoRequired<AutoPacketFactory> factory;

  // Obtain a packet for use with deferred decoration:
  auto packet = factory->NewPacket();

  // Satisfy the other decoration:
  packet->Decorate(Decoration<1>());

  {
    AutoCheckout<Decoration<0>> exterior;

    {
      AutoCheckout<Decoration<0>> checkout = packet->Checkout<Decoration<0>>();

      // Verify we can move the original type:
      AutoCheckout<Decoration<0>> checkoutMoved(std::move(checkout));

      // Verify no hits yet:
      EXPECT_FALSE(filterA->m_called) << "Filter called as a consequence of a checkout move operation";

      // Move the checkout a second time:
      exterior = std::move(checkoutMoved);
    }

    // Still no hits
    EXPECT_FALSE(filterA->m_called) << "Filter called before a decoration checkout expired";

    // Mark ready so we get committed:
    exterior.Ready();
  }

  // Verify a hit took place now
  EXPECT_LT(0, filterA->m_called) << "Filter was not called after all decorations were installed";
}

TEST_F(AutoFilterTest, RollbackCorrectness) {
  AutoRequired<FilterA> filterA;
  AutoRequired<AutoPacketFactory> factory;

  // Obtain a packet for use with deferred decoration:
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<1>());

  // Request and immediately allow the destruction of a checkout:
  packet->Checkout<Decoration<0>>();

  // Verify no hit took place--the checkout should have been cancelled:
  EXPECT_FALSE(filterA->m_called) << "Filter was called even though one decoration shouldn't have been available";

  // We should not be able to obtain another checkout of this decoration on this packet:
  EXPECT_ANY_THROW(packet->Checkout<Decoration<0>>()) << "An attempt to check out a decoration a second time should have failed";

  // We shouldn't be able to manually decorate, either:
  EXPECT_ANY_THROW(packet->Decorate(Decoration<0>())) << "An attempt to manually add a previously failed decoration succeeded where it should not have";
}

TEST_F(AutoFilterTest, VerifyAntiDecorate) {
  AutoRequired<FilterA> filterA;
  AutoRequired<AutoPacketFactory> factory;

  {
    // Obtain a new packet and mark an unsatisfiable decoration:
    auto packet = factory->NewPacket();
    packet->Unsatisfiable<Decoration<0>>();
    EXPECT_ANY_THROW(packet->Decorate(Decoration<0>())) << "Decoration succeeded on a decoration marked unsatisfiable";
  }

  {
    // Obtain a new packet and try to make a satisfied decoration unsatisfiable.
    auto packet = factory->NewPacket();
    packet->Decorate(Decoration<0>());
    EXPECT_ANY_THROW(packet->Unsatisfiable<Decoration<0>>()) << "Succeeded in marking an already-existing decoration as unsatisfiable";
  }
}

class OptionalResolveFilter {
public:
  size_t m_called;
  OptionalResolveFilter() : m_called(0) {
    DeclareAutoFilter(this, &OptionalResolveFilter::NextFilter1);
    DeclareAutoFilter(this, &OptionalResolveFilter::NextFilter2);
  };

  void AutoFilter(AutoPacket& pkt, optional_ptr<Decoration<0>> opt) {
    ++m_called;
    if (pkt.Has<Decoration<-1>>()) return; //Cannot attempt decorations

    if (opt) {
      //Called before final
      ASSERT_NO_THROW(pkt.Decorate(Decoration<-1>())) << "Decoration should be allowed";
      ASSERT_TRUE(pkt.Has<Decoration<-1>>());

      Decoration<-2> deco;
      ASSERT_NO_THROW(pkt.DecorateImmediate(deco)) << "Decoration should be allowed";
    } else {
      //Called during final
      ASSERT_NO_THROW(pkt.Decorate(Decoration<-1>())) << "Decoration should be blocked quietly";
      ASSERT_TRUE(pkt.Has<Decoration<-1>>()) << "Resolved AutoFilter calls should be able to add decorations";

      Decoration<-2> deco;
      ASSERT_NO_THROW(pkt.DecorateImmediate(deco)) << "Decoration should be blocked quietly";
    }
  }

  void NextFilter1(AutoPacket& pkt, const Decoration<-1>& dec) {
    ASSERT_TRUE(pkt.Has<Decoration<0>>()) << "NextFilter1 called during optional resolution";
  }

  void NextFilter2(AutoPacket& pkt, const Decoration<-2>& dec) {
    ASSERT_TRUE(pkt.Has<Decoration<0>>()) << "NextFilter2 called during optional resolution";
  }
};

TEST_F(AutoFilterTest, BlockResolveRecursion) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<OptionalResolveFilter> resolve;

  //Verify decoration success when optional is satisfied
  resolve->m_called = 0;
  {
    auto pkt = factory->NewPacket();
    pkt->Decorate(Decoration<0>());
    ASSERT_EQ(1, resolve->m_called) << "Failed to call AutoFilter with satisfied optional argument";
  }
  ASSERT_EQ(1, resolve->m_called) << "Multiple calls to AutoFilter with satisfied optional argument";

  //Verify decoration blocking when optional is resolved
  resolve->m_called = 0;
  {
    auto pkt = factory->NewPacket();
    ASSERT_EQ(0, resolve->m_called) << "Called AutoFilter with missing optional argument";
  }
  ASSERT_EQ(1, resolve->m_called) << "Failed to call AutoFilter with resolved optional argument";
}

/// <summary>
/// Replicate the static_assert in has_autofilter
/// </summary>
template<typename T>
struct good_autofilter {
  // Evaluates to false when T does not include an AutoFilter method with at least one argument.
  static const bool value = has_unambiguous_autofilter<T>::value;

  // This class has at least one AutoFilter method
  struct detect_ambiguous_autofilter : T, test_valid_autofilter {};

  // Ensures a compiler error when the identification of T::AutoFilter is ambiguous.
  // This cannot be in has_unambiguous_autofilter, since that would be recursive.
  static const bool test = value || has_unambiguous_autofilter<detect_ambiguous_autofilter>::value;
};

TEST_F(AutoFilterTest, VerifyReflexiveReciept) {
  AutoRequired<FilterA> filterA;
  AutoRequired<FilterC> filterC;
  AutoRequired<FilterD> filterD;
  AutoRequired<FilterE> filterE;

  //DEBUG: Expect compiler warning

  ASSERT_FALSE(good_autofilter<BadFilterA>::test) << "Failed to identify AutoFilter(void)";
  ASSERT_FALSE(good_autofilter<BadFilterB>::test) << "Failed to identify multiple definitions of AutoFilter";
  ASSERT_FALSE(good_autofilter<BadFilterC>::test) << "Failed to identify equivalent AutoFilter argument id types";

  AutoRequired<AutoPacketFactory> factory;

  // Obtain a packet first:
  auto packet = factory->NewPacket();

  // The mere act of obtaining a packet should have triggered filterD to be fired:
  EXPECT_LT(0, filterD->m_called) << "Trivial filter with AutoPacket argument was not called as expected";
  EXPECT_NO_THROW(packet->Get<Decoration<2>>()) << "Decoration on creation failed";

  // The mere act of obtaining a packet should have triggered filterD to be fired:
  EXPECT_LT(0, filterE->m_called) << "Trivial filter with no arguments was not called as expected";

  // The mere act of obtaining a packet should have triggered filterD to be fired:
  EXPECT_LT(0, filterD->m_called) << "Trivial filter was not called as expected";

  // Decorate--should satisfy filterC
  packet->Decorate(Decoration<0>());
  EXPECT_LT(0, filterC->m_called) << "FilterC should have been satisfied with one decoration";

  // FilterC should have also satisfied filterA:
  EXPECT_LT(0, filterA->m_called) << "FilterA should have been satisfied by FilterC";
}

TEST_F(AutoFilterTest, VerifyReferenceBasedInput) {
  std::shared_ptr<AutoPacket> packet;

  {
    AutoCreateContext sub;
    CurrentContextPusher pshr(sub);
    sub->Initiate();

    AutoRequired<AutoPacketFactory> factory;
    AutoRequired<FilterGen<Decoration<0>, Decoration<1>&>> makesDec1;

    // Create a packet and put decoration 0 on it:
    packet = factory->NewPacket();

    // No early call
    ASSERT_FALSE(makesDec1->m_called) << "Single-input autofilter was invoked prematurely";

    // Now we decorate, after ensuring an early call did not happen
    packet->Decorate(Decoration<0>());

    // Verify that our filter got called when its sole input was satisfied
    ASSERT_LT(0, makesDec1->m_called) << "Single-input autofilter was not called as expected";

    // Now make sure that the packet has the expected decoration:
    ASSERT_TRUE(packet->Has<Decoration<1>>());
    sub->SignalShutdown(false);
  }
}

class DeferredAutoFilter:
  public CoreThread
{
public:
  DeferredAutoFilter(void) :
    nReceived(0)
  {}

  Deferred AutoFilter(AutoPacket&, const Decoration<0>&) {
    // First packet is always delayed, this allows the dispatch queue to fill
    // up with packets, and triggers typical rundown behavior
    if(!nReceived)
      std::this_thread::sleep_for(std::chrono::milliseconds(10));

    nReceived++;
    return Deferred(this);
  }

  size_t nReceived;
};

TEST_F(AutoFilterTest, DeferredRecieptInSubContext) {
  static const size_t nPackets = 5;
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<DeferredAutoFilter>();

  std::vector<std::weak_ptr<AutoPacket>> allPackets;

  // Issue a packet before the subcontext is created, hold it for awhile:
  auto preissued = factory->NewPacket();
  preissued->Decorate(Decoration<0>());
  allPackets.push_back(preissued);

  {
    // Create subcontext
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);
    AutoRequired<DeferredAutoFilter> daf;

    // Now we initiate:
    ctxt->Initiate();

    // Issue a few packets, have them get picked up by the subcontext:
    for(size_t i = nPackets; i--;)
    {
      auto packet = factory->NewPacket();
      packet->Decorate(Decoration<0>());
      allPackets.push_back(packet);
    }

    // Terminate the subcontext, release references
    ctxt->SignalShutdown(true);
    ctxt->Wait();

    // Verify that the filter got all of the packets that it should have gotten:
    ASSERT_EQ(nPackets, daf->nReceived) << "AutoFilter did not get all packets that were pended to it";
  }

  // Release the preissued packet:
  preissued.reset();

  // Index of packets that were not expired at the time of detection
  std::set<size_t> notExpired;

  // Now verify that all of our packets are expired:
  for(size_t i = 0; i < allPackets.size(); i++)
    if(!allPackets[i].expired())
      notExpired.insert(i);

  if(!notExpired.empty()) {
    // Delay for a bit, see if they expire later:
    std::this_thread::sleep_for(std::chrono::seconds(1));
    for(size_t i = 0; i < allPackets.size(); i++)
      ASSERT_TRUE(allPackets[i].expired()) << "Packet " << i << " did not expire even after a delay that should have allowed it to expire";
   
    // They did, tell the user what didn't expire the first time around
    std::stringstream ss;
    for(auto index : notExpired)
      ss << index << " ";
    FAIL() << "These packets did not expire after all recipients went out of scope: " << ss.str();
  }
}

class HasAWeirdAutoFilterMethod {
public:
  HasAWeirdAutoFilterMethod(void):
    m_baseValue(101),
    m_called0(0),
    m_called1(0)
  {
  }

  void AutoFilter(Decoration<0>) {
    ASSERT_EQ(101, m_baseValue) << "AutoFilter entry base offset incorrectly computed";
    ++m_called0;
  }

  void AutoFilterFoo(Decoration<0>) {
    ASSERT_EQ(101, m_baseValue) << "AutoFilter entry base offset incorrectly computed";
    ++m_called1;
  }

  NewAutoFilter<decltype(&HasAWeirdAutoFilterMethod::AutoFilterFoo), &HasAWeirdAutoFilterMethod::AutoFilterFoo> af;
  const int m_baseValue;
  int m_called0;
  int m_called1;
};

TEST_F(AutoFilterTest, AnyAutoFilter) {
  AutoRequired<HasAWeirdAutoFilterMethod> t;
  AutoRequired<AutoPacketFactory> factory;
  auto packet = factory->NewPacket();

  packet->Decorate(Decoration<0>());
  ASSERT_TRUE(t->m_called0 == 1) << "Root AutoFilter method was not invoked as expected";
  ASSERT_TRUE(t->m_called1 == 1) << "Custom AutoFilter method was not invoked as expected";
}

class SimpleIntegerFilter
{
public:
  SimpleIntegerFilter(void) :
    hit(false)
  {}

  void AutoFilter(int val) {
    hit = true;
  }

  bool hit;
};

class DeferredIntegerFilter:
  public CoreThread
{
public:
  DeferredIntegerFilter(void) :
    CoreThread("DeferredIntegerFilter"),
    hit(false)
  {}

  Deferred AutoFilter(int val) {
    hit = true;
    return Deferred(this);
  }

  bool hit;
};

TEST_F(AutoFilterTest, SingleImmediate) {
  // Add a few filter entities
  AutoRequired<SimpleIntegerFilter> sif;
  AutoRequired<DeferredIntegerFilter> dif;

  AutoRequired<AutoPacketFactory> factory;

  {
    auto packet = factory->NewPacket();

    // Create an immediate-mode satisfaction
    int val = 101;
    packet->DecorateImmediate(val);

    // Verify we can't decorate this value a second time:
    ASSERT_ANY_THROW(packet->DecorateImmediate(val)) << "Expected an exception when a second attempt was made to attach a decoration";
  }
  ASSERT_EQ(0, factory->GetOutstanding()) << "Destroyed packet remains outstanding";

  static const int pattern = 1365; //1365 ~ 10101010101
  AutoRequired<FilterGen<Decoration<pattern>>> fgp;
  ASSERT_EQ(0, factory->GetOutstanding()) << "Outstanding packet count is correct after incrementing m_poolVersion due to AutoFilter addition";
  {
    auto packet = factory->NewPacket();
    Decoration<pattern> dec;
    packet->DecorateImmediate(dec);

    ASSERT_TRUE(fgp->m_called == 1) << "Filter should called " << fgp->m_called << " times, expected 1";
    ASSERT_TRUE(std::get<0>(fgp->m_args).i == pattern) << "Filter argument yielded " << std::get<0>(fgp->m_args).i << "expected " << pattern;
  }
  ASSERT_EQ(0, factory->GetOutstanding()) << "Destroyed packet remains outstanding";

  // Terminate enclosing context
  AutoCurrentContext()->SignalShutdown(true);

  ASSERT_TRUE(sif->hit) << "Simple filter was not invoked by immediate-mode decoration";
  ASSERT_FALSE(dif->hit) << "Deferred filter incorrectly invoked in an immediate mode decoration";
}

class DoesNothingWithSharedPointer:
  public CoreThread
{
public:
  DoesNothingWithSharedPointer(void):
    CoreThread("DoesNothingWithSharedPointer"),
    callCount(0)
  {}

  size_t callCount;

  Deferred AutoFilter(std::shared_ptr<const int>) {
    callCount++;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    return Deferred(this);
  }
};

TEST_F(AutoFilterTest, NoImplicitDecorationCaching) {
  AutoRequired<AutoPacketFactory> factory;
  auto ptr = std::make_shared<int>(1012);

  auto doesNothing = AutoRequired<DoesNothingWithSharedPointer>();

  // Generate a bunch of packets, all with the same shared pointer decoration:
  for(size_t i = 0; i < 100; i++) {
    // Decorate the packet, forget about it:
    auto packet = factory->NewPacket();
    packet->Decorate(ptr);
  }

  // Final lambda terminates the thread:
  *doesNothing += [doesNothing] { doesNothing->Stop(); };

  // Block for the thread to stop
  ASSERT_TRUE(doesNothing->WaitFor(std::chrono::seconds(10))) << "Thread did not finish processing packets in a timely fashion";

  // Ensure nothing got cached unexpectedly, and that the call count is precisely what we want
  ASSERT_EQ(100UL, doesNothing->callCount) << "The expected number of calls to AutoFilter were not made";
  ASSERT_TRUE(ptr.unique()) << "Cached packets (or some other cause) incorrectly held a reference to a shared pointer that should have expired";
}

TEST_F(AutoFilterTest, MultiImmediate) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<FilterGen<Decoration<0>, Decoration<1>>> fg;

  {
    auto packet = factory->NewPacket();
    packet->DecorateImmediate(
      Decoration<0>(),
      Decoration<1>()
    );

    // Verify the recipient got called
    ASSERT_EQ(1, fg->m_called) << "Filter not called during multisimultaneous immediate-mode decoration";
  }
  ASSERT_EQ(1, fg->m_called) << "Filter called repeatedly";
}

TEST_F(AutoFilterTest, ImmediateWithPrior) {
  AutoRequired<AutoPacketFactory> factory;

  // The filter which should get an immediate hit
  AutoRequired<FilterGen<Decoration<0>, Decoration<1>, Decoration<2>>> secondChanceImmed;

  {
    // Add a pre-decoration:
    auto packet = factory->NewPacket();
    packet->Decorate(Decoration<0>());

    // Now add immediate decorations to the remainder:
    packet->DecorateImmediate(Decoration<1>(), Decoration<2>());
    ASSERT_EQ(1, secondChanceImmed->m_called) << "Filter should have been saturated by an immediate call, but was not called as expected";
  }
  ASSERT_EQ(1, secondChanceImmed->m_called) << "Filter was called repeatedly";
}

TEST_F(AutoFilterTest, MultiImmediateComplex) {
  AutoRequired<AutoPacketFactory> factory;

  // All of the filters that we're adding
  AutoRequired<FilterGen<Decoration<0>>> fg1;
  AutoRequired<FilterGen<Decoration<1>, optional_ptr<Decoration<2>>>> fg2;
  AutoRequired<FilterGen<Decoration<0>, Decoration<1>>> fg3;
  AutoRequired<FilterGen<Decoration<0>, Decoration<2>>> fg4;

  {
    // The single immediate-mode decoration call, which should satisfy all but fg4
    auto packet = factory->NewPacket();
    packet->DecorateImmediate(
      Decoration<0>(),
      Decoration<1>()
    );

    // Validate expected behaviors:
    ASSERT_EQ(1, fg1->m_called) << "Trivial filter was not called as expected, even though Decoration<0> should have been available";
    ASSERT_EQ(1, fg2->m_called) << "Filter with an unsatisfied optional argument was not called";
    ASSERT_EQ(1, fg3->m_called) << "Saturated filter was not called as expected";
    ASSERT_EQ(0, fg4->m_called) << "Undersaturated filter was called even though it should not have been";
  }

  // Validate expected behaviors:
  ASSERT_EQ(1, fg1->m_called) << "Trivial filter was called repeatedly";
  ASSERT_EQ(1, fg2->m_called) << "Filter with an unsatisfied optional argument was called repeatedly";
  ASSERT_EQ(1, fg3->m_called) << "Saturated filter was not called as expected was called repeatedly";
  ASSERT_EQ(0, fg4->m_called) << "Undersaturated filter was called";
}

TEST_F(AutoFilterTest, PostHocSatisfactionAttempt) {
  AutoRequired<AutoPacketFactory> factory;

  // Filter that accepts two types, but one of the two will be DecorateImmediate'd too early
  AutoRequired<FilterGen<Decoration<0>, Decoration<1>>> fg1;
  AutoRequired<FilterGen<Decoration<2>, Decoration<1>>> fg2;

  auto packet1 = factory->NewPacket();
  packet1->DecorateImmediate(Decoration<0>());
  packet1->Decorate(Decoration<1>());

  ASSERT_FALSE(fg1->m_called) << "An AutoFilter was called when all of its inputs should not have been simultaneously available";

  packet1->DecorateImmediate(Decoration<2>());

  EXPECT_LT(0, fg2->m_called) << "An AutoFilter was not called when all of its inputs were simultaneously available";
}

TEST_F(AutoFilterTest, AutoOutTest) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<FilterOutA> foA;
  AutoRequired<FilterOutB> foB;
  {
    auto packet = factory->NewPacket();
    ASSERT_TRUE(foA->m_called == 1) << "An AutoFilter applied to one new packet with argument AutoPacket& was called " << foA->m_called << " times";
    ASSERT_TRUE(packet->Get<Decoration<0>>().i == 1) << "Decoration data was not initialized by AutoFilter call";
    ASSERT_TRUE(packet->Get<Decoration<1>>().i == 1) << "Decoration data was not appended by AutoFilter call";

    ASSERT_TRUE(foB->m_called == 1) << "An AutoFilter applied to one new packet without argument AutoPacket& reference was called " << foB->m_called << " times";
    ASSERT_TRUE(packet->Get<Decoration<2>>().i == 2) << "Decoration data was not appended by AutoFilter call";
  }
}

class WaitsForInternalLock:
  public CoreThread
{
public:
  std::mutex m_continueLock;

  Deferred AutoFilter(const Decoration<0>& dec) {
    return Deferred(this);
  }

  void Run(void) override {
    (std::lock_guard<std::mutex>)m_continueLock;
    CoreThread::Run();
  }
};

TEST_F(AutoFilterTest, NoDeferredImmediateSatisfaction) {
  // Create a waiter, then obtain its lock before sending it off:
  AutoRequired<WaitsForInternalLock> wfil;
  std::lock_guard<std::mutex> lk(wfil->m_continueLock);

  // Now create a packet that we will DecorateImmediate with our decoration:
  AutoRequired<AutoPacketFactory> factory;
  auto packet = factory->NewPacket();
  packet->DecorateImmediate(Decoration<0>());

  // Verify that the thread did not receive anything:
  ASSERT_EQ(0UL, wfil->GetDispatchQueueLength()) << "Deferred AutoFilter incorrectly received an immediate-mode decoration";
}

class AcceptsConstReference {
public:
  AcceptsConstReference(void) :
  m_called(0)
  {}

  int m_called;

  void AutoFilter(const int& dataIn) {
    ++m_called;
  }
};

class AcceptsSharedPointer {
public:
  AcceptsSharedPointer(void) :
  m_called(0)
  {}

  int m_called;

  void AutoFilter(std::shared_ptr<const int> dataIn) {
    ++m_called;
  }
};

TEST_F(AutoFilterTest, SharedPtrCollapse) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<AcceptsConstReference> constr_filter;
  AutoRequired<AcceptsSharedPointer> shared_filter;

  int constr_int = 0;
  std::shared_ptr<int> shared_int = std::make_shared<int>(0);
  ASSERT_TRUE(static_cast<bool>(shared_int));

  // Decorate(type X) calls AutoFilter(const type& X)
  // Decorate(type X) calls AutoFilter(shared_ptr<type> X)
  {
    auto packet = factory->NewPacket();
    packet->Decorate(constr_int);
    ASSERT_EQ(1, constr_filter->m_called) << "Called const reference method " << constr_filter->m_called << " times";
    ASSERT_EQ(1, shared_filter->m_called) << "Called shared pointer method " << shared_filter->m_called << " times";
  }
  constr_filter->m_called = 0;
  shared_filter->m_called = 0;

  // Decorate(shared_ptr<type> X) calls AutoFilter(const type& X)
  // Decorate(shared_ptr<type> X) calls AutoFilter(shared_ptr<type> X)
  // NOTE: Decorate(shared_ptr<T> X) shares ownership of X instead of copying.
  {
    auto packet = factory->NewPacket();
    packet->Decorate(shared_int);
    ASSERT_EQ(1, constr_filter->m_called) << "Called const reference method " << constr_filter->m_called << " times";
    ASSERT_EQ(1, shared_filter->m_called) << "Called shared pointer method " << shared_filter->m_called << " times";
    ASSERT_FALSE(shared_int.unique()) << "Argument of Decorate should be shared, not copied, when possible";
  }
  constr_filter->m_called = 0;
  shared_filter->m_called = 0;

  // DecorateImmediate(type X) calls AutoFilter(const type& X)
  // DecorateImmediate(type X) DOES NOT CALL AutoFilter(shared_ptr<type> X)
  // NOTE: This case is invalid, since DecorateImmediate assumes no validity of X after the function call,
  // so the construction of a shared_ptr from &X would violate the contract of shared_ptr type.
  // If an AutoFilter method assumed the validity of shared_ptr<type> Y, a copy could be made that might
  // become invalid.
  // NOTE: DecorateImmediate(shared_ptr<type> X) is unnecessary (a static_assert will be called).
  // Decorate(shared_ptr<type> X) will share the reference to X instead of making a copy.
  {
    auto packet = factory->NewPacket();
    packet->DecorateImmediate(constr_int);
    ASSERT_EQ(1, constr_filter->m_called) << "Called const reference method " << constr_filter->m_called << " times";
    ASSERT_EQ(0, shared_filter->m_called) << "Called shared pointer method " << shared_filter->m_called << " times";
  }
  constr_filter->m_called = 0;
  shared_filter->m_called = 0;
}

TEST_F(AutoFilterTest, SharedPointerAliasingRules) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<FilterGen<std::shared_ptr<const int>>> genFilter1;
  AutoRequired<FilterGen<int>> genFilter2;

  auto packet = factory->NewPacket();
  packet->Decorate<int>(55);

  ASSERT_EQ(1UL, genFilter1->m_called) << "AutoFilter accepting a shared pointer was not called as expected";
  ASSERT_EQ(1UL, genFilter2->m_called) << "AutoFilter accepting a decorated type was not called as expected";
}

TEST_F(AutoFilterTest, AutoSelfUpdateTest) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<AutoSelfUpdate<Decoration<0>>> filter;

  {
    auto packet = factory->NewPacket();
    ASSERT_TRUE(packet->Has<AutoSelfUpdate<Decoration<0>>::prior_object>()) << "Missing status update from AutoSelfUpdate";
    Decoration<0> mod_deco;
    mod_deco.i = 1;
    packet->Decorate(mod_deco);
    Decoration<0> get_deco = *filter; //Implicit cast from AutoSelfUpdate
    ASSERT_EQ(1, get_deco.i) << "AutoSelfUpdate did not update";
  }

  {
    auto packet = factory->NewPacket();
    Decoration<0> get_deco = packet->Get<AutoSelfUpdate<Decoration<0>>::prior_object>(); //Implicit cast from prior_object
    ASSERT_EQ(1, get_deco.i) << "Status updated yielded incorrect prior";
  }
}

TEST_F(AutoFilterTest, AutoSelfUpdateTwoContexts) {
  AutoCreateContext contextA;
  {
    CurrentContextPusher pusher(contextA);
    ASSERT_NO_THROW(AutoRequired<AutoSelfUpdate<Decoration<0>>>()) << "Failed to create AutoSelfUpdate in contextA";
  }

  AutoCreateContext contextB;
  {
    CurrentContextPusher pusher(contextB);
    ASSERT_NO_THROW(AutoRequired<AutoSelfUpdate<Decoration<0>>>()) << "Failed to create AutoSelfUpdate in contextB";
  }
}

TEST_F(AutoFilterTest, AutoTimeStampTest) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<AutoTimeStamp> stamper;
  ASSERT_TRUE(factory->NewPacket()->Has<AutoTimeStamp::time>()) << "Failed to stamp packet on initialization";
}

TEST_F(AutoFilterTest, WaitWhilePacketOutstanding) {
  AutoRequired<AutoPacketFactory> factory;
  auto packet = factory->NewPacket();

  AutoCurrentContext ctxt;
  ctxt->SignalShutdown();
  ASSERT_FALSE(ctxt->Wait(std::chrono::milliseconds(100))) << "Wait incorrectly returned while packets were outstanding";
  packet.reset();
  ASSERT_TRUE(ctxt->Wait(std::chrono::milliseconds(1))) << "Wait incorrectly timed out when nothing should have been running";
}

class DecoratesAndAcceptsNothing:
  public CoreThread
{
public:
  Deferred AutoFilter(Decoration<0>& dec) {
    dec.i = 105;
    return Deferred(this);
  }
};

TEST_F(AutoFilterTest, DeferredDecorateOnly) {
  AutoCurrentContext ctxt;
  AutoRequired<DecoratesAndAcceptsNothing> daan;
  AutoRequired<AutoPacketFactory> factory;

  auto packet = factory->NewPacket();

  ctxt->SignalShutdown();
  ASSERT_TRUE(daan->WaitFor(std::chrono::seconds(5)));

  const Decoration<0>* dec;
  ASSERT_TRUE(packet->Get(dec)) << "Deferred decorator didn't attach a decoration to an issued packet";
  ASSERT_EQ(105, dec->i) << "Deferred decorate-only AutoFilter did not properly attach before context termination";
}

TEST_F(AutoFilterTest, MicroAutoFilterTests) {
  int extVal = -1;
  MicroAutoFilter<void, const int&> makeImmediate(
  [&extVal] (const int& getVal) {
    extVal = getVal;
  });
  int setVal = 1;
  makeImmediate.AutoFilter(setVal);
  ASSERT_EQ(1, extVal);
}

struct MultiFilter01 {
  std::list<int> call_vals;
  void Call0(const Decoration<0>& arg) { call_vals.push_back(arg.i); }
  void Call1(const Decoration<1>& arg) { call_vals.push_back(arg.i); }

  MultiFilter01() {
    // Constructor wraps each method in an AutoFilter call
    DeclareAutoFilter(this, &MultiFilter01::Call0);
    DeclareAutoFilter(this, &MultiFilter01::Call1);
  }
};

TEST_F(AutoFilterTest, DeclareAutoFilterTest) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<MultiFilter01> mf01;

  auto packetA = factory->NewPacket();
  packetA->Decorate(Decoration<0>());
  ASSERT_EQ(1, mf01->call_vals.size()) << "Failed to call AutoFilter wrapped method";
  ASSERT_EQ(0, mf01->call_vals.back()) << "Calling value was not propagated";

  auto packetB = factory->NewPacket();
  packetB->Decorate(Decoration<1>());
  ASSERT_EQ(2, mf01->call_vals.size()) << "Failed to call AutoFilter wrapped method";
  ASSERT_EQ(1, mf01->call_vals.back()) << "Calling value was not propagated";

  packetA->Decorate(Decoration<1>(2));
  ASSERT_EQ(3, mf01->call_vals.size()) << "Failed to call AutoFilter wrapped method";
  ASSERT_EQ(2, mf01->call_vals.back()) << "Calling value was not propagated";
}

void FilterFunction(const Decoration<0>& typeIn, auto_out<Decoration<1>> typeOut) {
  typeOut->i += 1 + typeIn.i;
}

typedef std::function<void(const Decoration<0>&, auto_out<Decoration<1>>)> FilterFunctionType;

TEST_F(AutoFilterTest, FunctionDecorationTest) {
  // AddRecipient that is an instance of std::function f : a -> b
  // This must be satisfied by decoration of type a,
  // independent of the order of decoration.

  AutoRequired<AutoPacketFactory> factory;

  //Decoration with data first
  {
    auto packet = factory->NewPacket();
    packet->Decorate(Decoration<0>());
    packet->AddRecipient(FilterFunctionType(FilterFunction));
    const Decoration<1>* getdec;
    ASSERT_TRUE(packet->Get(getdec)) << "Decoration function was not called";
  }

  //Decoration with function first
  //NOTE: This test also catches failures to flush temporary subscriber information
  {
    auto packet = factory->NewPacket();
    packet->AddRecipient(FilterFunctionType(FilterFunction));
    packet->Decorate(Decoration<0>());
    const Decoration<1>* getdec;
    ASSERT_TRUE(packet->Get(getdec)) << "Decoration function was not called";
  }
}

TEST_F(AutoFilterTest, FunctionDecorationLambdaTest) {
  AutoRequired<AutoPacketFactory> factory;

  //Decoration with function first
  {
    auto packet = factory->NewPacket();
    int addType = 1;
    packet->AddRecipient(FilterFunctionType([addType] (const Decoration<0>& typeIn, auto_out<Decoration<1>> typeOut) {
      typeOut->i += 1 + typeIn.i;
    }));
    packet->Decorate(Decoration<0>());
    const Decoration<1>* getdec;
    ASSERT_TRUE(packet->Get(getdec)) << "Decoration function was not called";
    ASSERT_EQ(1+addType, getdec->i) << "Increment was not applied";
  }
}

typedef std::function<void(auto_out<Decoration<0>>)> InjectorFunctionType;

TEST_F(AutoFilterTest, FunctionInjectorTest) {
  AutoRequired<AutoPacketFactory> factory;

  auto packet = factory->NewPacket();
  int addType = 1;
  packet->AddRecipient(InjectorFunctionType([addType](auto_out<Decoration<0>> typeOut) {
    typeOut->i += addType;
  }));
  const Decoration<0>* getdec;
  ASSERT_TRUE(packet->Get(getdec)) << "Decoration function was not called";
  ASSERT_EQ(0+addType, getdec->i) << "Increment was not applied";
}

typedef std::function<void(const Decoration<1>&)> ExtractorFunctionType;

TEST_F(AutoFilterTest, FunctionExtractorTest) {
  AutoRequired<AutoPacketFactory> factory;

  auto packet = factory->NewPacket();
  int extType = -1;
  packet->AddRecipient(ExtractorFunctionType([&extType](const Decoration<1>& typeIn) {
    extType = typeIn.i;
  }));
  packet->Decorate(Decoration<1>());
  ASSERT_EQ(1, extType) << "Decoration type was not extracted";
}

class FilterDiamondIn:
  public ContextMember
{
public:
  int m_called;
  FilterDiamondIn(void) : m_called(0) {}
  void AutoFilter(auto_out<Decoration<0>> init) {
    ++m_called;
    init->i = 1;
  }
};

class FilterDiamondA:
  public ContextMember
{
public:
  int m_called;
  FilterDiamondA(void) : m_called(0) {}
  void AutoFilter(const Decoration<0>& in, auto_out<Decoration<1>> out) {
    ++m_called;
    out->i = 2;
  }
};

class FilterDiamondB:
  public ContextMember
{
public:
  int m_called;
  FilterDiamondB(void) : m_called(0) {}
  void AutoFilter(const Decoration<0>& in, auto_out<Decoration<1>> out) {
    ++m_called;
    out->i = 3;
  }
};

class FilterDiamondOut:
  public ContextMember
{
public:
  int m_called;
  Decoration<1> m_inLast;
  FilterDiamondOut(void) : m_called(0) {}
  void AutoFilter(const Decoration<1>& in) {
    ++m_called;
    m_inLast = in;
  }
};

class DiamondFilter:
  public ContextMember
{
public:
  DiamondFilter() {
    Reset();
  }

  void Reset() {
    In->m_called = 0;
    A->m_called = 0;
    B->m_called = 0;
    Out->m_called = 0;
    In_expected = 0;
    A_expected = 0;
    B_expected = 0;
    Out_expected = 0;
  }

  void Verify() {
    ASSERT_EQ(In_expected, In->m_called) << "Diamond Filter I called " << In->m_called << " expected " << In_expected;
    ASSERT_EQ(A_expected, A->m_called) << "Diamond Filter A called " << A->m_called << " expected " << A_expected;
    ASSERT_EQ(B_expected, B->m_called) << "Diamond Filter B called " << B->m_called << " expected " << B_expected;
    ASSERT_EQ(Out_expected, Out->m_called) << "Diamond Filter O called " << Out->m_called << " expected " << Out_expected;
  }

  AutoRequired<FilterDiamondIn> In;
  AutoRequired<FilterDiamondA> A;
  AutoRequired<FilterDiamondB> B;
  AutoRequired<FilterDiamondOut> Out;

  int In_expected;
  int A_expected;
  int B_expected;
  int Out_expected;
};

TEST_F(AutoFilterTest, AutoEdgeTest) {
  AutoCurrentContext()->Initiate();
  AutoRequired<AutoPacketFactory> factory;
  DiamondFilter diamond;

  //Diamond configuration will throw on creation of the packet, preventing any calls
  ASSERT_THROW(factory->NewPacket(), std::runtime_error) << "Failed to anticipate broadcast collision";
  diamond.Verify();
  diamond.Reset();

  //Incorrect pipe declarations will throw
  ASSERT_THROW(factory->BroadcastDataIn<FilterDiamondIn>(&typeid(Decoration<1>),false), std::runtime_error) << "Failed to throw missing type";
  ASSERT_THROW(factory->BroadcastDataIn<FilterDiamondA>(&typeid(Decoration<1>),false), std::runtime_error) << "Failed to throw incorrect orientation";

  //Permit DiamondA to use pipes only, which will prevent data collision, even though all filters are called.
  factory->BroadcastDataOut<FilterDiamondA>(&typeid(Decoration<1>),false);
  ASSERT_NO_THROW(factory->NewPacket()) << "Incorrect data collision";
  ++diamond.In_expected;
  ++diamond.A_expected;
  ++diamond.B_expected;
  ++diamond.Out_expected;
  diamond.Verify();
  diamond.Reset();

  //Permit DiamondIn to use pipes only, which will prevent data propagation
  factory->BroadcastDataOut<FilterDiamondIn>(&typeid(Decoration<0>),false);
  factory->NewPacket();
  ++diamond.In_expected;
  diamond.Verify();
  diamond.Reset();

  //Connect DiamondIn to DiamondA
  factory->PipeData<FilterDiamondIn, FilterDiamondA>(&typeid(Decoration<0>));
  factory->NewPacket();
  ++diamond.In_expected;
  ++diamond.A_expected;
  diamond.Verify();
  diamond.Reset();

  //Connect DiamondA to DiamondOut, which will cause a collision
  //PROBLEM: Exception is thrown, but termination in ~AutoCheckout is not caught
  /*
  factory->PipeData<FilterDiamondIn, FilterDiamondB>(); //Pipe all correctly oriented types
  factory->PipeData<FilterDiamondA, FilterDiamondOut>(); //Pipe all correctly oriented types
  ASSERT_THROW(factory->NewPacket(), std::runtime_error) << "Data failed to collide";
   */
}

template<int num = 0>
class FilterOD0 {
public:
  void AutoFilter(auto_out<Decoration<0>> out) {
    out->i = 0;
  }
};

class FilterID0 {
public:
  void AutoFilter(const Decoration<0>& in) {}
};

TEST_F(AutoFilterTest, VerifyMergedOutputs) {
  AutoRequired<AutoPacketFactory> factory;

  AutoRequired<FilterOD0<0>> f0D0_broadcast;
  AutoRequired<FilterOD0<1>> f1D0_pipe_merge;
  AutoRequired<FilterOD0<2>> f1D0_pipe_fID0;
  AutoRequired<FilterID0> fID0_pipe_f2D0;

  AutoRequired<AutoMerge<Decoration<0>>> merge;

  // No broadcast from f1D0 and f2D0
  factory->BroadcastDataIn<FilterID0>(nullptr,false);
  factory->BroadcastDataOut<FilterOD0<1>>(nullptr,false);
  factory->BroadcastDataOut<FilterOD0<2>>(nullptr,false);

  // f0D0 is broadcast, and will be received by merge
  // f1D0 -> merge only
  // f2D0 -> fID0 only
  factory->PipeData<FilterOD0<1>, AutoMerge<Decoration<0>>>();
  factory->PipeData<FilterOD0<2>, FilterID0>();

  AutoMerge<Decoration<0>>::merge_data extracted;
  {
    std::shared_ptr<AutoPacket> packet;
    ASSERT_NO_THROW(packet = factory->NewPacket()) << "Multi-Decorate hit an exception";
    packet->Decorate(AutoMerge<Decoration<0>>::merge_call([&extracted](const AutoMerge<Decoration<0>>::merge_data& data) {
      extracted = data;
    }));

    ASSERT_TRUE(packet->Has<Decoration<0>>()) << "Broadcast data should be present";
    ASSERT_FALSE(packet->Has<Decoration<0>>(typeid(FilterOD0<0>))) << "Sourced data should be absent from broadcasting source";
    ASSERT_TRUE(packet->Has<Decoration<0>>(typeid(FilterOD0<1>))) << "Sourced data should be present from non-broadcasting source";
    ASSERT_TRUE(packet->Has<Decoration<0>>(typeid(FilterOD0<2>))) << "Sourced data should be present from non-broadcasting source";

    // Final-Call methods
    // TODO: Make these throw if called early!
    ASSERT_EQ(1, packet->HasAll<Decoration<0>>()) << "Single Broadcast source only";
    ASSERT_EQ(1, packet->HasAll<Decoration<0>>(typeid(AutoMerge<Decoration<0>>))) << "Single Piped source only";
    ASSERT_EQ(1, packet->HasAll<Decoration<0>>(typeid(FilterID0))) << "Single Piped source only";
  }
  ASSERT_EQ(2, extracted.size()) << "Should collect 1 broadcast & 1 pipe";
}

TEST_F(AutoFilterTest, VerifyForwardAll) {
  const std::shared_ptr<Decoration<0>>* contents = nullptr;
  AutoRequired<AutoPacketFactory> factory;
  std::shared_ptr<AutoPacket> toPacket = factory->NewPacket();
  {
    std::shared_ptr<AutoPacket> fromPacket = factory->NewPacket();
    fromPacket->Decorate(Decoration<0>(1));
    contents = nullptr; //contents does not own referenced data
    ASSERT_TRUE(fromPacket->Get(contents)) << "ForwardedAll failed to move data";
    ASSERT_EQ(1, (*contents)->i) << "ForwardedAll failed to move data";

    fromPacket->ForwardAll(toPacket);
    contents = nullptr; //contents does not own referenced data
    ASSERT_TRUE(toPacket->Get(contents)) << "ForwardedAll failed to move data";
    ASSERT_EQ(1, (*contents)->i) << "ForwardedAll failed to move data";
  }
  contents = nullptr; //contents does not own referenced data
  ASSERT_TRUE(toPacket->Get(contents)) << "Forwarded data is not persistent";
  ASSERT_EQ(1, (*contents)->i) << "Forwarded data is not persistent";
}

class JunctionDeferred01:
  public CoreThread
{
public:
  Deferred AutoFilter(const Decoration<0>& in, auto_out<Decoration<1>> out) {
    // Wait for AutoStile method to exit
    AutoCurrentContext()->Wait(std::chrono::milliseconds(10));
    
    out->i = in.i;
    return Deferred(this);
  }
};

class SlaveContext {};
class MasterContext {};

TEST_F(AutoFilterTest, VerifyContextStile) {
  // Stile injects Decoration<0> and extracts Decoration<1>
  typedef AutoStile<
    const Decoration<0>&,
    auto_out<Decoration<1>>,
    auto_out<Decoration<2>>
  > test_stile;
  std::shared_ptr<test_stile> stile;
  std::shared_ptr<AutoPacketFactory> master_factory;

  AutoCreateContextT<SlaveContext> slave_context;
  {
    CurrentContextPusher pusher(slave_context);
    AutoRequired<AutoPacketFactory>();
    AutoRequired<JunctionDeferred01>();
    slave_context->Initiate();
  }

  AutoCreateContextT<MasterContext> master_context;
  {
    CurrentContextPusher pusher(master_context);
    master_factory = AutoRequired<AutoPacketFactory>();
    stile = AutoRequired<test_stile>();
    master_context->Initiate();
  }

  stile->Leash(slave_context);
  {
    CurrentContextPusher pusher(master_context);
    std::shared_ptr<AutoPacket> master_packet;
    master_packet = master_factory->NewPacket();
    int init = -1;
    master_packet->Decorate(Decoration<0>(init));

    // Create a last-call so that completion of slave context
    // is verified.
    master_packet->AddRecipient(std::function<void(const AutoPacket&)>([init](const AutoPacket& master_final){
      const Decoration<1>* out1;
      ASSERT_TRUE(master_final.Get(out1)) << "Stile failed to send & retrieve data";
      ASSERT_EQ(init, out1->i) << "Output was not from junction in slave context";
      const Decoration<2>* out2;
      ASSERT_FALSE(master_final.Get(out2)) << "Decoration<2> was not produced in slave context";
    }));
  }
}

class Junction01 {
public:
  void AutoFilter(const Decoration<0>& in, auto_out<Decoration<1>> out) {
    out->i = in.i;
  }
};

TEST_F(AutoFilterTest, VerifyStileExtractAll) {
  // Stile injects Decoration<0> and extracts Decoration<1>
  std::shared_ptr<AutoStile<const Decoration<0>&>> stile;
  std::shared_ptr<AutoPacketFactory> master_factory;

  AutoCreateContextT<SlaveContext> slave_context;
  {
    CurrentContextPusher pusher(slave_context);
    AutoRequired<AutoPacketFactory>();
    AutoRequired<Junction01>();
    slave_context->Initiate();
  }

  AutoCreateContextT<MasterContext> master_context;
  {
    CurrentContextPusher pusher(master_context);
    master_factory = AutoRequired<AutoPacketFactory>();
    stile = AutoRequired<AutoStile<const Decoration<0>&>>();
    master_context->Initiate();
  }

  stile->Leash(slave_context);
  {
    CurrentContextPusher pusher(master_context);
    std::shared_ptr<AutoPacket> master_packet;
    master_packet = master_factory->NewPacket();
    master_packet->Decorate(Decoration<0>());
    ASSERT_TRUE(master_packet->Has<Decoration<1>>()) << "Stile failed to send & retrieve data";
  }
}

TEST_F(AutoFilterTest, ExtractMergedData) {
  std::shared_ptr<AutoStile<const AutoMergeStile<Decoration<0>>::merge_call&>> stile;
  std::shared_ptr<AutoPacketFactory> master_factory;

  AutoCreateContextT<SlaveContext> slave_context;
  {
    CurrentContextPusher pusher(slave_context);

    AutoRequired<AutoPacketFactory>();
    AutoRequired<FilterOD0<0>> f0D0;
    AutoRequired<FilterOD0<1>> f1D0;

    slave_context->Initiate();
  }

  AutoCreateContextT<MasterContext> master_context;
  {
    CurrentContextPusher pusher(master_context);

    master_factory = AutoRequired<AutoPacketFactory>();
    stile = AutoRequired<AutoStile<const AutoMergeStile<Decoration<0>>::merge_call&>>();
    AutoConstruct<AutoMergeStile<Decoration<0>>> merge(*slave_context);

    merge->GetMerge()->PipeToMerge<FilterOD0<0>>();
    merge->GetMerge()->PipeToMerge<FilterOD0<1>>();

    master_context->Initiate();
  }

  stile->Leash(slave_context);
  {
    CurrentContextPusher pusher(master_context);
    std::shared_ptr<AutoPacket> master_packet;
    master_packet = master_factory->NewPacket();
    master_packet->Decorate(Decoration<0>());
    const AutoMergeStile<Decoration<0>>::merge_data* data = nullptr;
    ASSERT_TRUE(master_packet->Get(data)) << "Stile failed to send & retrieve data";
    ASSERT_EQ(2, data->size()) << "Merge failed to gather all data";
  }
}
