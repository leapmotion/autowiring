// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"
#include <autowiring/AutoPacket.h>
#include <autowiring/AutoPacketFactory.h>
#include <autowiring/Deferred.h>
#include <autowiring/demangle.h>
#include <autowiring/ObjectPool.h>
#include <autowiring/AutoSelfUpdate.h>
#include <autowiring/AutoTimeStamp.h>
#include <autowiring/SatCounter.h>
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

TEST_F(AutoFilterTest, GetOutstandingTest) {
  AutoRequired<AutoPacketFactory> factory;

  {
    auto packet = factory->NewPacket();
    ASSERT_EQ(1UL, factory->GetOutstandingPacketCount()) << "Factory outstanding count mismatch";
  }

  ASSERT_EQ(0UL, factory->GetOutstandingPacketCount()) << "Factory outstanding did not go to zero after releasing the only outstanding packet";
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

  ASSERT_TRUE(bFound) << "Failed to find an added subscriber";

  // Obtain a packet from the factory:
  auto packet = factory->NewPacket();

  // Decorate with one instance:
  packet->Decorate(Decoration<0>());

  // Verify that no hit takes place with inadequate decoration:
  ASSERT_FALSE(filterA->m_called) << "Filter called prematurely with insufficient decoration";

  // Now decorate with the other requirement of the filter:
  packet->Decorate(Decoration<1>());

  // A hit should have taken place at this point:
  ASSERT_LT(0, filterA->m_called) << "Filter was not called even though it was fully satisfied";
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
  ASSERT_NO_THROW(packet->Decorate(Decoration<1>(2))) << "Decoration with parent type conflicts with derived type";
  ASSERT_EQ(1, filterA->m_called) << "AutoFilter was not called when all arguments were available";
  ASSERT_EQ(2, filterA->m_one.i) << "AutoFilter was called using derived type instead of parent";
}

int DoTheThingAndStuff(void) {
  return 999;
}

TEST_F(AutoFilterTest, VerifyAutoOut) {
  AutoRequired<AutoPacketFactory> factory;

  *factory += [](auto_out<Decoration<0>> out) {
    *out = Decoration<0>(1);
  };

  std::shared_ptr<AutoPacket> packet = factory->NewPacket();
  const Decoration<0>* result0 = nullptr;
  ASSERT_TRUE(packet->Get(result0)) << "Output missing";
  ASSERT_EQ(result0->i, 1) << "Output incorrect";
}

TEST_F(AutoFilterTest, VerifyAutoOutPooled) {
  AutoRequired<AutoPacketFactory> factory;
  ObjectPool<Decoration<0>> pool;

  *factory += [&](auto_out<Decoration<0>> out) {
    auto p = pool();
    p->i = 1;
    *out = p;
  };

  std::shared_ptr<AutoPacket> packet = factory->NewPacket();
  const Decoration<0>* result0 = nullptr;
  ASSERT_TRUE(packet->Get(result0)) << "Output missing";
  ASSERT_EQ(result0->i, 1) << "Output incorrect";
}

TEST_F(AutoFilterTest, VerifyNoMultiDecorate) {
  AutoRequired<FilterA> filterA;
  AutoRequired<AutoPacketFactory> factory;

  // Obtain a packet and attempt redundant introduction:
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>());
  ASSERT_ANY_THROW(packet->Decorate(Decoration<0>())) << "Redundant decoration did not throw an exception as expected";

  // Verify that a call has not yet been made
  ASSERT_FALSE(filterA->m_called) << "A call made on an idempotent packet decoration";

  // Now finish saturating the filter and ensure we get a call:
  packet->Decorate(Decoration<1>());
  ASSERT_LT(0, filterA->m_called) << "Filter was not called after being fully satisfied";

  //NOTE: A typedef will throw an exception
  typedef Decoration<0> isDeco0type;
  ASSERT_ANY_THROW(packet->Decorate(isDeco0type())) << "Typedef failed to throw exception";

  //NOTE: A shared_ptr to an existing type will throw an exception
  auto sharedDeco0 = std::make_shared<Decoration<0>>();
  ASSERT_ANY_THROW(packet->Decorate(sharedDeco0)) << "Reduction of shared_ptr to base type failed";

  //NOTE: Inheritance will not throw an exception
  class ofDeco0alias: public Decoration<0> {};
  try {
    packet->Decorate(ofDeco0alias());
  } catch (...) {
    FAIL() << "Class with inheritance from existing decoration reinterpreted as child type";
  }

  // Verify that DecorateImmedaite also yields an exception
  Decoration<0> localDeco0;
  ASSERT_ANY_THROW(packet->DecorateImmediate(localDeco0)) << "Redundant immediate decoration did not throw an exception as expected";
  ASSERT_ANY_THROW(packet->DecorateImmediate(Decoration<2>(), Decoration<2>())) << "Repeated type in immediate decoration was not identified as an error";
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
  ASSERT_FALSE(filterB->m_called) << "A call was made to a thread which should not have been able to process it";

  // Wake up the barrier and post a quit message:
  filterB->Continue();
  *filterB += [&filterB] { filterB->Stop(); };
  filterB->Wait();

  // Verify that the filter method has been called
  ASSERT_LT(0, filterB->m_called) << "A deferred filter method was not called as expected";
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
      factory->RemoveSubscriber(filterA);
    }

    // Verify that unsubscription STILL does not result in expiration:
    ASSERT_FALSE(filterAWeak.expired()) << "A subscriber expired before all packets on that subscriber were satisfied";

    //Create a new packet after having removed the only filter on it.
    auto packet2 = factory->NewPacket();
    ASSERT_FALSE(packet2->HasSubscribers<Decoration<0>>()) << "A packet had subscriptions after the only subscriber was removed.";

    // Satisfy the packet:
    packet->Decorate(Decoration<0>());
    packet->Decorate(Decoration<1>());

    auto packet3 = factory->NewPacket();
    ASSERT_FALSE(packet3->HasSubscribers<Decoration<0>>()) << "A packet had subscriptions after the only subscriber was removed.";
  }

  // Filter should be expired now:
  ASSERT_TRUE(filterAWeak.expired()) << "Subscriber was still left outstanding even though all references should be gone";
}

TEST_F(AutoFilterTest, VerifyAntiDecorate) {
  AutoRequired<FilterA> filterA;
  AutoRequired<AutoPacketFactory> factory;

  {
    // Obtain a new packet and mark an unsatisfiable decoration:
    auto packet = factory->NewPacket();
    packet->MarkUnsatisfiable<Decoration<0>>();
    ASSERT_ANY_THROW(packet->Decorate(Decoration<0>())) << "Incorrectly allowed a decoration to be added to a packet when that decoration was unsatisfiable";
  }

  {
    // Obtain a new packet and try to make a satisfied decoration unsatisfiable.
    auto packet = factory->NewPacket();
    packet->Decorate(Decoration<0>());
    ASSERT_NO_THROW(packet->MarkUnsatisfiable<Decoration<0>>()) << "Failed to expunge a decoration from a packet";
  }
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

  AutoRequired<AutoPacketFactory> factory;

  // Obtain a packet first:
  auto packet = factory->NewPacket();

  // The mere act of obtaining a packet should have triggered filterD to be fired:
  ASSERT_LT(0, filterD->m_called) << "Trivial filter with AutoPacket argument was not called as expected";
  ASSERT_NO_THROW(packet->Get<Decoration<2>>()) << "Decoration on creation failed";

  // The mere act of obtaining a packet should have triggered filterD to be fired:
  ASSERT_LT(0, filterE->m_called) << "Trivial filter with no arguments was not called as expected";

  // The mere act of obtaining a packet should have triggered filterD to be fired:
  ASSERT_LT(0, filterD->m_called) << "Trivial filter was not called as expected";

  // Decorate--should satisfy filterC
  packet->Decorate(Decoration<0>());
  ASSERT_LT(0, filterC->m_called) << "FilterC should have been satisfied with one decoration";

  // FilterC should have also satisfied filterA:
  ASSERT_LT(0, filterA->m_called) << "FilterA should have been satisfied by FilterC";
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

TEST_F(AutoFilterTest, ZeroArgumentAutoFilter) {
  AutoRequired<AutoPacketFactory> factory;

  int called1 = 0;
  *factory += [&] { called1++; };
  factory->NewPacket();

  ASSERT_EQ(1, called1) << "Zero-argument AutoFilter method was not invoked as expected";
}

TEST_F(AutoFilterTest, ByValueDecoration) {
  AutoRequired<AutoPacketFactory> factory;

  int called0 = 0;
  int value = 101;
  *factory += [&called0, value](Decoration<0>) {
    // Check to ensure that the value was transferred in correctly
    ASSERT_EQ(101, value) << "AutoFilter entry base offset incorrectly computed";
    ++called0;
  };

  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>());
  ASSERT_EQ(1, called0) << "Root AutoFilter method was not invoked as expected";
}

class SimpleIntegerFilter
{
public:
  void AutoFilter(int val) {
    hit = true;
  }

  bool hit = false;
};

class DeferredIntegerFilter:
  public CoreThread
{
public:
  DeferredIntegerFilter(void) :
    CoreThread("DeferredIntegerFilter")
  {}

  Deferred AutoFilter(int val) {
    hit = true;
    return Deferred(this);
  }

  bool hit = false;
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
  ASSERT_EQ(0, factory->GetOutstandingPacketCount()) << "Destroyed packet remains outstanding";

  static const int pattern = 1365; //1365 ~ 10101010101
  AutoRequired<FilterGen<Decoration<pattern>>> fgp;
  ASSERT_EQ(0, factory->GetOutstandingPacketCount()) << "Outstanding packet count is correct after incrementing m_poolVersion due to AutoFilter addition";
  {
    auto packet = factory->NewPacket();
    Decoration<pattern> dec;
    packet->DecorateImmediate(dec);

    ASSERT_TRUE(fgp->m_called == 1) << "Filter should called " << fgp->m_called << " times, expected 1";
    ASSERT_TRUE(autowiring::get<0>(fgp->m_args).i == pattern) << "Filter argument yielded " << autowiring::get<0>(fgp->m_args).i << "expected " << pattern;
  }
  ASSERT_EQ(0, factory->GetOutstandingPacketCount()) << "Destroyed packet remains outstanding";

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
    CoreThread("DoesNothingWithSharedPointer")
  {}

  size_t callCount = 0;

  Deferred AutoFilter(std::shared_ptr<const int>) {
    callCount++;
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
    ASSERT_EQ(1, fg3->m_called) << "Saturated filter was not called as expected";
    ASSERT_EQ(0, fg4->m_called) << "Undersaturated filter was called even though it should not have been";
  }

  // Validate expected behaviors:
  ASSERT_EQ(1, fg1->m_called) << "Trivial filter was called repeatedly";
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

  ASSERT_LT(0, fg2->m_called) << "An AutoFilter was not called when all of its inputs were simultaneously available";
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

    const Decoration<2>* pDec2;
    ASSERT_TRUE(packet->Get(pDec2)) << "Decoration<2> was not present on the packet";
    ASSERT_TRUE(pDec2->i == 2) << "Decoration data was not appended by AutoFilter call";
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
  auto then = std::chrono::high_resolution_clock::now();
  auto packet = factory->NewPacket();
  const std::chrono::high_resolution_clock::time_point* pLater;
  ASSERT_TRUE(packet->Get(pLater)) << "Failed to stamp packet on initialization";

  auto now = std::chrono::high_resolution_clock::now();

  // Ensure that events happened in the order expected
  ASSERT_LE(then, *pLater) << "Packet timestamp is from a point in time before the test was started";
  ASSERT_LE(*pLater, now) << "Packet timestampe is dated after test conclusion";
}

TEST_F(AutoFilterTest, WaitWhilePacketOutstanding) {
  AutoRequired<AutoPacketFactory> factory;
  auto packet = factory->NewPacket();

  AutoCurrentContext ctxt;
  ctxt->SignalShutdown();
  ASSERT_FALSE(ctxt->Wait(std::chrono::milliseconds(1))) << "Wait incorrectly returned while packets were outstanding";
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

class DQueueSharedPointer:
  public DispatchQueue
{
public:
  size_t callCount = 0;

  Deferred AutoFilter(std::shared_ptr<const int>) {
    callCount++;
    return Deferred(this);
  }
};

TEST_F(AutoFilterTest, DQueueAutoFilterTest) {
  AutoRequired<AutoPacketFactory> factory;
  auto ptr = std::make_shared<int>(1012);

  AutoRequired<DQueueSharedPointer> dQueueSharedPtr;

  // Generate a bunch of packets, all with the same shared pointer decoration:
  for(size_t i = 0; i < 100; i++) {
    // Decorate the packet, forget about it:
    auto packet = factory->NewPacket();
    packet->Decorate(ptr);
  }

  ASSERT_EQ(0UL, dQueueSharedPtr->callCount);

  dQueueSharedPtr->DispatchAllEvents();

  // Ensure nothing got cached unexpectedly, and that the call count is precisely what we want
  ASSERT_EQ(100UL, dQueueSharedPtr->callCount) << "The expected number of calls to AutoFilter were not made";
}

class MyInheritingAutoFilter:
  public FilterGen<std::vector<int>>
{};

TEST_F(AutoFilterTest, AutoFilterInBaseClass) {
  AutoRequired<MyInheritingAutoFilter> d;
  AutoRequired<AutoPacketFactory> f;

  // Packet decoration shouldn't cause problems by itself
  auto packet = f->NewPacket();
  packet->Decorate(std::vector<int>{0, 1, 2});

  // Trivial validation that we got something back
  ASSERT_EQ(1UL, d->m_called) << "Filter defined in base class was not called the expected number of times";
}

class MyInheritingAutoFilterNoAlias:
  public ContextMember,
  public FilterGen<std::vector<int>>
{};

TEST_F(AutoFilterTest, AutoFilterInBaseClassNoAlias) {
  AutoRequired<MyInheritingAutoFilterNoAlias> d;
  AutoRequired<AutoPacketFactory> f;

  // Packet decoration shouldn't cause problems by itself
  auto packet = f->NewPacket();
  packet->Decorate(std::vector<int>{0, 1, 2});

  // Trivial validation that we got something back
  ASSERT_EQ(1UL, d->m_called) << "Filter defined in base class in an Object-inheriting type was not called the expected number of times";
}

TEST_F(AutoFilterTest, PacketTeardownNotificationCheck) {
  AutoRequired<AutoPacketFactory> factory;
  auto called = std::make_shared<bool>(false);

  {
    auto packet = factory->NewPacket();
    packet->AddTeardownListener([called] { *called = true; });
    ASSERT_FALSE(*called) << "Teardown listener called before packet was destroyed";
  }

  ASSERT_TRUE(*called) << "Teardown listener was not called after packet destruction";
  ASSERT_TRUE(called.unique()) << "Teardown listener lambda function was leaked";
}

struct ContextChecker:
  ContextMember
{
  void AutoFilter(int i) {
    ++m_called;
    ASSERT_EQ(AutoCurrentContext(), GetContext()) << "AutoFilter not called with the current context set to packet's context";
  }

  int m_called = 0;
};

TEST_F(AutoFilterTest, CurrentContextCheck) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<ContextChecker> filter;

  {
    CurrentContextPusher pshr((AutoCreateContext()));
    auto packet = factory->NewPacket();
    packet->Decorate(42);
  }

  ASSERT_EQ(1, filter->m_called) << "AutoFilter called incorrect number of times";
}

TEST_F(AutoFilterTest, AutoOut0) {
  AutoRequired<AutoPacketFactory> factory;
  {
    auto packet = factory->NewPacket();
    packet->Decorate(123.45);
    ASSERT_EQ(1, packet->GetDecorationTypeCount()) << "Did not get the expected number of decorations.";
    ASSERT_EQ(123.45, packet->Get<double>());
    *packet += [](const double &x, auto_out<int> y) {
      *y = static_cast<int>(x);
    };
    ASSERT_EQ(2, packet->GetDecorationTypeCount()) << "Did not get the expected number of decorations.";
    ASSERT_EQ(123, packet->Get<int>());
  }
}

TEST_F(AutoFilterTest, AutoOut1) {
  AutoRequired<AutoPacketFactory> factory;
  {
    auto packet = factory->NewPacket();
    // Same as AutoOut0 but add the filter first.
    *packet += [](const double &x, auto_out<int> y) {
      *y = static_cast<int>(x);
    };
    packet->Decorate(123.45);
    ASSERT_EQ(2, packet->GetDecorationTypeCount()) << "Did not get the expected number of decorations.";
    ASSERT_EQ(123.45, packet->Get<double>());
    ASSERT_EQ(123, packet->Get<int>());
  }
}

#include <iostream>

std::string decoration_status_string(DispositionState s) {
  switch (s) {
  case DispositionState::Unsatisfied:
    return "Unsatisfied";
  case DispositionState::PartlySatisfied:
    return "PartlySatisfied";
  case DispositionState::Complete:
    return "Complete";
  }
  return "";
}

void print_decorations(const std::string &label, const AutoPacket::t_decorationMap &d) {
  std::cout << label << " {\n";
  for (auto it : d) {
    std::cout << "    " << autowiring::demangle(it.first.id.block->ti) << " : " << decoration_status_string(it.second.m_state) << '\n';
  }
  std::cout << "}\n";
}

TEST_F(AutoFilterTest, AutoOut2) {
  
  // NOTE: this test uses implementation details of DecorationDisposition, which is not part of the public API,
  // and should be refactored once that part is cleaned up (in particular, AutoPacket::GetDecorations is
  // what is exposing that, and should be moved into autowiring::dbg, out of the public API for AutoPacket).
  
  AutoRequired<AutoPacketFactory> factory;
  {
    auto packet = factory->NewPacket();
    auto d = packet->GetDecorations();
    ASSERT_EQ(0, d.size()) << "Unexpected number of AutoFilter parameters.";
    
    packet->Decorate(123.45);
    d = packet->GetDecorations();
    ASSERT_EQ(1, d.size()) << "Unexpected number of AutoFilter parameters.";
    ASSERT_EQ(DispositionState::Complete, d[DecorationKey(auto_id_t<double>(),0)].m_state) << "Incorrect `double` decoration disposition.";
    ASSERT_EQ(1, d[DecorationKey(auto_id_t<double>(),0)].m_decorations.size()) << "Incorrect `double` decoration disposition.";

    bool filter_0_called = false;
    auto filter_0 = [&filter_0_called](const double &x, auto_out<int> y) {
      filter_0_called = true;
    };
    bool filter_1_called = false;
    auto filter_1 = [&filter_1_called](int y) {
      filter_1_called = true;
    };
    
    ASSERT_FALSE(filter_0_called) << "We expected filter_0 to not have been called by now.";
    ASSERT_FALSE(filter_1_called) << "We expected filter_1 to not have been called by now.";
    
    *packet += filter_0;
    
    ASSERT_TRUE(filter_0_called) << "We expected filter_0 to have been called by now.";
    ASSERT_FALSE(filter_1_called) << "We expected filter_0 to not have been called by now.";
    d = packet->GetDecorations();
    ASSERT_EQ(2, d.size()) << "Unexpected number of AutoFilter parameters.";
    ASSERT_EQ(DispositionState::Complete, d[DecorationKey(auto_id_t<double>(),0)].m_state) << "Incorrect `double` decoration disposition.";
    ASSERT_EQ(1, d[DecorationKey(auto_id_t<double>(),0)].m_decorations.size()) << "Incorrect `double` decoration disposition.";
    // Being Complete and having no decorations indicates that it has been MarkUnsatisfiable()'d.
    ASSERT_EQ(DispositionState::Complete, d[DecorationKey(auto_id_t<int>(),0)].m_state) << "Incorrect `int` decoration disposition.";
    ASSERT_EQ(0, d[DecorationKey(auto_id_t<int>(),0)].m_decorations.size()) << "Incorrect `int` decoration disposition.";

    // Because the auto_out<int> was never assigned to, it went unsatisfiable, so this filter should never be called.
    *packet += filter_1;
    ASSERT_TRUE(filter_0_called) << "We expected filter_0 to have been called by now.";
    ASSERT_FALSE(filter_1_called) << "We expected filter_0 to not have been called by now.";
  }
}

TEST_F(AutoFilterTest, AutoOut3) {
  
  // NOTE: this test uses implementation details of DecorationDisposition, which is not part of the public API,
  // and should be refactored once that part is cleaned up (in particular, AutoPacket::GetDecorations is
  // what is exposing that, and should be moved into autowiring::dbg, out of the public API for AutoPacket).
  
  AutoRequired<AutoPacketFactory> factory;
  {
    auto packet = factory->NewPacket();
    auto d = packet->GetDecorations();
    ASSERT_EQ(0, d.size()) << "Unexpected number of AutoFilter parameters.";

    bool filter_1_called = false;
    auto filter_1 = [&filter_1_called](int y) {
      filter_1_called = true;
    };
    *packet += filter_1;
    
    packet->Decorate(123.45);
    d = packet->GetDecorations();
    ASSERT_EQ(2, d.size()) << "Unexpected number of AutoFilter parameters.";
    ASSERT_EQ(DispositionState::Complete, d[DecorationKey(auto_id_t<double>(),0)].m_state) << "Incorrect `double` decoration disposition.";
    ASSERT_EQ(1, d[DecorationKey(auto_id_t<double>(),0)].m_decorations.size()) << "Incorrect `double` decoration disposition.";
    ASSERT_EQ(DispositionState::Unsatisfied, d[DecorationKey(auto_id_t<int>(),0)].m_state) << "Incorrect `int` decoration disposition.";
    ASSERT_EQ(0, d[DecorationKey(auto_id_t<int>(),0)].m_decorations.size()) << "Incorrect `int` decoration disposition.";

    bool filter_0_called = false;
    auto filter_0 = [&filter_0_called](const double &x, auto_out<int> y) {
      filter_0_called = true;
    };
    
    ASSERT_FALSE(filter_0_called) << "We expected filter_0 to not have been called by now.";
    ASSERT_FALSE(filter_1_called) << "We expected filter_1 to not have been called by now.";
    
    *packet += filter_0;
    
    ASSERT_TRUE(filter_0_called) << "We expected filter_0 to have been called by now.";
    ASSERT_FALSE(filter_1_called) << "We expected filter_0 to not have been called by now.";
    d = packet->GetDecorations();
    ASSERT_EQ(2, d.size()) << "Unexpected number of AutoFilter parameters.";
    ASSERT_EQ(DispositionState::Complete, d[DecorationKey(auto_id_t<double>(),0)].m_state) << "Incorrect `double` decoration disposition.";
    ASSERT_EQ(1, d[DecorationKey(auto_id_t<double>(),0)].m_decorations.size()) << "Incorrect `double` decoration disposition.";
    // Being Complete and having no decorations indicates that it has been MarkUnsatisfiable()'d.
    ASSERT_EQ(DispositionState::Complete, d[DecorationKey(auto_id_t<int>(),0)].m_state) << "Incorrect `int` decoration disposition.";
    ASSERT_EQ(0, d[DecorationKey(auto_id_t<int>(),0)].m_decorations.size()) << "Incorrect `int` decoration disposition.";

    // Because the auto_out<int> was never assigned to, it went unsatisfiable, so this filter should never be called.
    ASSERT_TRUE(filter_0_called) << "We expected filter_0 to have been called by now.";
    ASSERT_FALSE(filter_1_called) << "We expected filter_0 to not have been called by now.";
  }
}

TEST_F(AutoFilterTest, AutoOut4) {
  
  // NOTE: this test uses implementation details of DecorationDisposition, which is not part of the public API,
  // and should be refactored once that part is cleaned up (in particular, AutoPacket::GetDecorations is
  // what is exposing that, and should be moved into autowiring::dbg, out of the public API for AutoPacket).
  
  AutoRequired<AutoPacketFactory> factory;
  {
    auto packet = factory->NewPacket();
    auto d = packet->GetDecorations();
    ASSERT_EQ(0, d.size()) << "Unexpected number of AutoFilter parameters.";

    bool filter_1_called = false;
    auto filter_1 = [&filter_1_called](int y) {
      filter_1_called = true;
    };
    *packet += filter_1;
    
    packet->Decorate(123.45);
    d = packet->GetDecorations();
    ASSERT_EQ(2, d.size()) << "Unexpected number of AutoFilter parameters.";
    ASSERT_EQ(DispositionState::Complete, d[DecorationKey(auto_id_t<double>(),0)].m_state) << "Incorrect `double` decoration disposition.";
    ASSERT_EQ(1, d[DecorationKey(auto_id_t<double>(),0)].m_decorations.size()) << "Incorrect `double` decoration disposition.";
    ASSERT_EQ(DispositionState::Unsatisfied, d[DecorationKey(auto_id_t<int>(),0)].m_state) << "Incorrect `int` decoration disposition.";
    ASSERT_EQ(0, d[DecorationKey(auto_id_t<int>(),0)].m_decorations.size()) << "Incorrect `int` decoration disposition.";

    bool filter_0_called = false;
    auto_out<int> ao;
    auto filter_0 = [&ao, &filter_0_called](const double &x, auto_out<int> y) {
      ao = std::move(y);
      filter_0_called = true;
    };
    
    ASSERT_FALSE(filter_0_called) << "We expected filter_0 to not have been called by now.";
    ASSERT_FALSE(filter_1_called) << "We expected filter_1 to not have been called by now.";
    
    *packet += filter_0;
    
    ASSERT_TRUE(filter_0_called) << "We expected filter_0 to have been called by now.";
    ASSERT_FALSE(filter_1_called) << "We expected filter_0 to not have been called by now.";
    d = packet->GetDecorations();
    ASSERT_EQ(2, d.size()) << "Unexpected number of AutoFilter parameters.";
    ASSERT_EQ(DispositionState::Complete, d[DecorationKey(auto_id_t<double>(),0)].m_state) << "Incorrect `double` decoration disposition.";
    ASSERT_EQ(1, d[DecorationKey(auto_id_t<double>(),0)].m_decorations.size()) << "Incorrect `double` decoration disposition.";

    // Being Complete and having no decorations indicates that it has been MarkUnsatisfiable()'d.
    ASSERT_EQ(DispositionState::Unsatisfied, d[DecorationKey(auto_id_t<int>(),0)].m_state) << "Incorrect `int` decoration disposition.";
    ASSERT_EQ(0, d[DecorationKey(auto_id_t<int>(),0)].m_decorations.size()) << "Incorrect `int` decoration disposition.";

    // Assign to ao, thereby decorating the packet with int.
    *ao = 42;
    ao.reset();
    
    ASSERT_TRUE(filter_0_called) << "We expected filter_0 to have been called by now.";
    ASSERT_TRUE(filter_1_called) << "We expected filter_0 to have been called by now.";
    d = packet->GetDecorations();
    ASSERT_EQ(2, d.size()) << "Unexpected number of AutoFilter parameters.";
    ASSERT_EQ(DispositionState::Complete, d[DecorationKey(auto_id_t<double>(),0)].m_state) << "Incorrect `double` decoration disposition.";
    ASSERT_EQ(1, d[DecorationKey(auto_id_t<double>(),0)].m_decorations.size()) << "Incorrect `double` decoration disposition.";
    ASSERT_EQ(DispositionState::Complete, d[DecorationKey(auto_id_t<int>(),0)].m_state) << "Incorrect `int` decoration disposition.";
    ASSERT_EQ(1, d[DecorationKey(auto_id_t<int>(),0)].m_decorations.size()) << "Incorrect `int` decoration disposition.";
  }
}
