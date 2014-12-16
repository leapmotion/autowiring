// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"
#include <autowiring/AutoPacket.h>
#include <autowiring/AutoPacketFactory.h>
#include <autowiring/Deferred.h>
#include <autowiring/NewAutoFilter.h>
#include <autowiring/DeclareAutoFilter.h>
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
    ASSERT_EQ(1UL, factory->GetOutstanding()) << "Factory outstanding count mismatch";
  }

  ASSERT_EQ(0UL, factory->GetOutstanding()) << "Factory outstanding did not go to zero after releasing the only outstanding packet";
}

TEST_F(AutoFilterTest, VerifyDescendentAwareness) {
  // Create a packet while the factory has no subscribers:
  AutoRequired<AutoPacketFactory> parentFactory;
  std::shared_ptr<AutoPacket> firstPacket = parentFactory->NewPacket();

  // Verify subscription-free status:
  EXPECT_FALSE(firstPacket->HasSubscribers<Decoration<0>>()) << "Subscription exists where one should not have existed";

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
    EXPECT_FALSE(firstPacket->HasSubscribers<Decoration<0>>()) << "Subscription was incorrectly, retroactively added to a packet";

    //Create a packet where a subscriber exists only in a subcontext
    strongPacket = parentFactory->NewPacket();
    std::shared_ptr<AutoPacket> holdPacket = parentFactory->NewPacket();
    weakPacket = holdPacket;
    EXPECT_TRUE(strongPacket->HasSubscribers<Decoration<0>>()) << "Packet lacked expected subscription from subcontext";
    EXPECT_TRUE(weakPacket.lock()->HasSubscribers<Decoration<0>>()) << "Packet lacked expected subscription from subcontext";
  }
  EXPECT_TRUE(weakPacket.expired()) << "Packet was not destroyed when it's subscribers were removed";
  EXPECT_FALSE(filterChecker.expired()) << "Packet keeping subcontext member alive";

  // Verify the second packet will no longer have subscriptions  -
  // normally removing a subscriber would mean the packet still has the subscriber, but
  // in this case, the subscriber was actually destroyed so the packet has lost a subscriber.
  EXPECT_TRUE(strongPacket->HasSubscribers<Decoration<0>>()) << "Missing subscriber from destroyed subcontext";

  // Call the subscriber... this will either succeed or segfault.
  strongPacket->Decorate(Decoration<0>());
  strongPacket->Decorate(Decoration<1>());
  EXPECT_TRUE(strongPacket->HasSubscribers<Decoration<0>>()) << "Calling a subscriber should not remove it";
  {
    std::shared_ptr<FilterA> holdFilter = filterChecker.lock();
    ASSERT_EQ(1, holdFilter->m_called) << "Subcontext filter was not called";
  }

  // Create a packet after the subcontext has been destroyed
  auto lastPacket = parentFactory->NewPacket();
  EXPECT_FALSE(lastPacket->HasSubscribers<Decoration<0>>()) << "Subscription was incorrectly, retroactively added to a packet";

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

class FilterOutPooled {
  ObjectPool<Decoration<0>> m_pool;
public:
  void AutoFilter(auto_out<Decoration<0>> out) {
    out = m_pool();
    out-> i = 1;
  }
};

TEST_F(AutoFilterTest, VerifyAutoOutPooled) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<FilterOutPooled> out;
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
    FilterGather_AutoGather(this, &FilterGather<out, in>::AutoGather),
    m_called_out(0),
    m_called_in(0),
    m_out(out),
    m_in(in)
  {}

  void AutoFilter(AutoPacket& packet) {
    ++m_called_out;
    packet.Decorate(Decoration<out>(m_out));
  }


  NewAutoFilter FilterGather_AutoGather;
  void AutoGather(const Decoration<in>& input) {
    ++m_called_in;
    m_in = input.i;
  }

  int m_called_out;
  int m_called_in;
  int m_out;
  int m_in;
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
    af(this, &HasAWeirdAutoFilterMethod::AutoFilterFoo),
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

  NewAutoFilter af;
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
