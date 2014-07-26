// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoFilterTest.hpp"
#include "TestFixtures/Decoration.hpp"
#include <autowiring/AutoPacket.h>
#include <autowiring/AutoPacketFactory.h>
#include <autowiring/NewAutoFilter.h>
#include THREAD_HEADER

TEST_F(AutoFilterTest, VerifyDescendentAwareness) {
  AutoCurrentContext()->Initiate();

  // Create a packet while the factory has no subscribers:
  AutoRequired<AutoPacketFactory> parentFactory;
  auto packet1 = parentFactory->NewPacket();

  // Verify subscription-free status:
  EXPECT_FALSE(packet1->HasSubscribers<Decoration<0>>()) << "Subscription exists where one should not have existed";

  std::shared_ptr<AutoPacket> packet2;
  std::weak_ptr<AutoPacket> packet3;
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

    //Create a packet where a subscriber exists only in a subcontext
    packet2 = parentFactory->NewPacket();
    auto strongPacket3 = parentFactory->NewPacket();
    packet3 = strongPacket3;
    EXPECT_TRUE(packet2->HasSubscribers<Decoration<0>>()) << "Packet lacked expected subscription from subcontext";
    EXPECT_TRUE(packet3.lock()->HasSubscribers<Decoration<0>>()) << "Packet lacked expected subscription from subcontext";
  }
  EXPECT_TRUE(packet3.expired()) << "Packet was not destroyed when it's subscribers were removed";
  EXPECT_FALSE(filterChecker.expired()) << "Packet keeping subcontext member alive";

  // Create a packet after the subcontext has been destroyed
  auto packet4 = parentFactory->NewPacket();
  EXPECT_FALSE(packet4->HasSubscribers<Decoration<0>>()) << "Subscription exists where one should not have existed";

  // Verify the first packet still does not have subscriptions:
  EXPECT_FALSE(packet1->HasSubscribers<Decoration<0>>()) << "Subscription was incorrectly, retroactively added to a packet";

  packet2->Decorate(Decoration<0>());

  // Verify the second packet will no longer have subscriptions  -
  // normally removing a subscriber would mean the packet still has the subscriber, but
  // in this case, the subscriber was actually destroyed so the packet has lost a subscriber.
  EXPECT_TRUE(packet2->HasSubscribers<Decoration<0>>()) << "Packet lacked an expected subscription";

  // Verify the third one does not:
  EXPECT_FALSE(packet4->HasSubscribers<Decoration<0>>()) << "Subscription was incorrectly, retroactively added to a packet";

  packet2.reset();
  EXPECT_TRUE(filterChecker.expired()) << "Subscriber didn't expire after packet was reset.";
}

TEST_F(AutoFilterTest, VerifySimpleFilter) {
  AutoCurrentContext()->Initiate();

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

TEST_F(AutoFilterTest, VerifyOptionalFilter) {
  AutoCurrentContext()->Initiate();
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
    ASSERT_TRUE(fgB->m_called == 0) << "An AutoFilter was called " << fgA->m_called << " times when an optional input was unresolved";
    ASSERT_TRUE(fgC->m_called == 0) << "An AutoFilter was called " << fgA->m_called << " times when a required input was not available";

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
  }

  ASSERT_TRUE(fgA->m_called == 1) << "An AutoFilter was called " << fgA->m_called << " times when all required inputs were available";
  ASSERT_TRUE(fgB->m_called == 1) << "An AutoFilter was called " << fgB->m_called << " times when all required inputs were available";
  ASSERT_TRUE(fgC->m_called == 0) << "An AutoFilter was called " << fgC->m_called << " times when a required input was not available";
  ASSERT_TRUE(fgD->m_called == 1) << "An AutoFilter was called " << fgD->m_called << " times when all required inputs were available";
}

TEST_F(AutoFilterTest, VerifyNoMultiDecorate) {
  AutoCurrentContext()->Initiate();

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
}

TEST_F(AutoFilterTest, VerifyInterThreadDecoration) {
  AutoRequired<FilterB> filterB;
  AutoRequired<AutoPacketFactory> factory;
  AutoCurrentContext ctxt;

  // Kick off all threads:
  ctxt->Initiate();

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
  AutoCurrentContext()->Initiate();
  AutoRequired<AutoPacketFactory> factory;

  std::weak_ptr<FilterA> filterAWeak;

  {
    std::shared_ptr<AutoPacket> packet;
    {
      // Create the filter and subscribe it
      std::shared_ptr<FilterA> filterA(new FilterA);
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

TEST_F(AutoFilterTest, VerifyCheckout) {
  AutoCurrentContext()->Initiate();
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
  AutoCurrentContext()->Initiate();
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
  AutoCurrentContext()->Initiate();

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
  ASSERT_FALSE(good_autofilter<BadFilterA>::test) << "Failed to identify multiple defintiions of AutoFilter";

  AutoRequired<AutoPacketFactory> factory;

  AutoCurrentContext()->Initiate();

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

TEST_F(AutoFilterTest, DeferredRecieptInSubContext)
{
  AutoCurrentContext()->Initiate();

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

  // Now verify that all of our packets are expired:
  for(auto cur : allPackets)
    ASSERT_TRUE(cur.expired()) << "Packet did not expire after all recipients went out of scope";
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
  AutoCurrentContext()->Initiate();
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

  AutoCurrentContext()->Initiate();

  AutoRequired<AutoPacketFactory> factory;

  {
    auto packet = factory->NewPacket();

    // Create an immediate-mode satisfaction
    int val = 101;
    packet->DecorateImmediate(val);

    // Verify we can't decorate this value a second time:
    ASSERT_ANY_THROW(packet->DecorateImmediate(val)) << "Expected an exception when a second attempt was made to attach a decoration";
  }

  static const int pattern = 1365; //1365 ~ 10101010101
  AutoRequired<FilterGen<Decoration<pattern>>> fgp;
  {
    auto packet = factory->NewPacket();
    Decoration<pattern> dec;
    packet->DecorateImmediate(dec);

    ASSERT_TRUE(fgp->m_called == 1) << "Filter should called " << fgp->m_called << " times, expected 1";
    ASSERT_TRUE(std::get<0>(fgp->m_args).i == pattern) << "Filter argument yielded " << std::get<0>(fgp->m_args).i << "expected " << pattern;
  }

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

  Deferred AutoFilter(std::shared_ptr<int>) {
    callCount++;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    return Deferred(this);
  }
};

TEST_F(AutoFilterTest, NoImplicitDecorationCaching) {
  AutoCurrentContext()->Initiate();
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
  AutoCurrentContext()->Initiate();
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<FilterGen<Decoration<0>, Decoration<1>>> fg;

  auto packet = factory->NewPacket();

  packet->DecorateImmediate(
    Decoration<0>(),
    Decoration<1>()
  );

  // Verify the recipient got called
  ASSERT_LT(0, fg->m_called) << "Filter not called during multisimultaneous immediate-mode decoration";
}

TEST_F(AutoFilterTest, ImmediateWithPrior) {
  AutoCurrentContext()->Initiate();
  AutoRequired<AutoPacketFactory> factory;

  // The filter which should get an immediate hit
  AutoRequired<FilterGen<Decoration<0>, Decoration<1>, Decoration<2>>> secondChanceImmed;

  // Add a pre-decoration:
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>());

  // Now add immediate decorations to the remainder:
  packet->DecorateImmediate(Decoration<1>(), Decoration<2>());
  ASSERT_LT(0, secondChanceImmed->m_called) << "Filter should have been saturated by an immediate call, but was not called as expected";
}

TEST_F(AutoFilterTest, MultiImmediateComplex) {
  AutoCurrentContext()->Initiate();
  AutoRequired<AutoPacketFactory> factory;

  // All of the filters that we're adding
  AutoRequired<FilterGen<Decoration<0>>> fg1;
  AutoRequired<FilterGen<Decoration<1>, optional_ptr<Decoration<2>>>> fg2;
  AutoRequired<FilterGen<Decoration<0>, Decoration<1>>> fg3;
  AutoRequired<FilterGen<Decoration<0>, Decoration<2>>> fg4;

  auto packet = factory->NewPacket();

  // The single immediate-mode decoration call, which should satisfy all filters
  packet->DecorateImmediate(
    Decoration<0>(),
    Decoration<1>()
  );

  // Validate expected behaviors:
  ASSERT_LT(0, fg1->m_called) << "Trivial filter was not called as expected, even though Decoration<0> should have been available";
  ASSERT_LT(0, fg2->m_called) << "Filter with an unsatisfied optional argument was not called";
  ASSERT_LT(0, fg3->m_called) << "Saturated filter was not called as expected";
  ASSERT_EQ(0, fg4->m_called) << "Undersaturated filter was called even though it should not have been";
}

TEST_F(AutoFilterTest, PostHocSatisfactionAttempt) {
  AutoCurrentContext()->Initiate();
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
  AutoCurrentContext()->Initiate();
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
  AutoCurrentContext()->Initiate();

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

  void AutoFilter(std::shared_ptr<int> dataIn) {
    ++m_called;
  }
};

TEST_F(AutoFilterTest, SharedPtrCollapse) {
  AutoCurrentContext()->Initiate();
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

class AcceptsSharedPointerAndReference {
public:
  AcceptsSharedPointerAndReference(void) :
    m_called(false)
  {}

  bool m_called;

  void AutoFilter(const int& v, std::shared_ptr<int> vShared) {
    m_called = true;
    ASSERT_EQ(&v, vShared.get()) << "Shared pointer to a type did not point to the same address as the type reference proper";
  }
};

TEST_F(AutoFilterTest, SharedPointerRecieptRules) {
  AutoCurrentContext()->Initiate();
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<AcceptsSharedPointerAndReference> filter;
  AutoRequired<FilterGen<std::shared_ptr<int>>> genFilter1;
  AutoRequired<FilterGen<int>> genFilter2;

  auto packet = factory->NewPacket();
  packet->Decorate<int>(55);

  ASSERT_TRUE(filter->m_called) << "Redundant-input filter was not called";
  ASSERT_EQ(1UL, genFilter1->m_called) << "AutoFilter accepting a shared pointer was not called as expected";
  ASSERT_EQ(1UL, genFilter2->m_called) << "AutoFilter accepting a decorated type was not called as expected";
}

TEST_F(AutoFilterTest, SharedPointerAliasingRules) {
  AutoCurrentContext()->Initiate();
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<AcceptsSharedPointerAndReference> filter;
  AutoRequired<FilterGen<std::shared_ptr<int>>> genFilter1;
  AutoRequired<FilterGen<int>> genFilter2;

  auto packet = factory->NewPacket();
  packet->Decorate(std::make_shared<int>(56));

  ASSERT_TRUE(filter->m_called) << "Redundant-input filter was not called";
  ASSERT_EQ(1UL, genFilter1->m_called) << "AutoFilter accepting a shared pointer was not called as expected";
  ASSERT_EQ(1UL, genFilter2->m_called) << "AutoFilter accepting a decorated type was not called as expected";
}
