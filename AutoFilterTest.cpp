#include "stdafx.h"
#include "AutoFilterTest.h"
#include "AutoPacket.h"
#include "AutoPacketFactory.h"
#include "TestFixtures/Decoration.h"

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
  for(const auto& cur : factory->GetSubscriberVector())
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
  EXPECT_TRUE(filterA->m_called) << "Filter was not called even though it was fully satisfied";
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
  EXPECT_TRUE(filterA->m_called) << "Filter was not called after being fully satisfied";
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
  filterB->m_barr.wait();
  *filterB += [&filterB] { filterB->Stop(); };
  filterB->Wait();

  // Verify that the filter method has been called
  EXPECT_TRUE(filterB->m_called) << "A deferred filter method was not called as expected";
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
  AutoCurrentContext()->Initiate();
  AutoRequired<FilterA> filterA;
  AutoRequired<AutoPacketFactory> factory;

  // Obtain a packet for use with deferred decoration:
  auto packet = factory->NewPacket();

  {
    // Verify that an unsubscribed decoration returns a correct checkout:
    auto unused = packet->Checkout<Decoration<4>>();
    EXPECT_FALSE(unused) << "Checkout incorrectly generated for unsubscribed decoration";
  }

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
  EXPECT_TRUE(filterA->m_called) << "Filter was not called after all decorations were installed";
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

TEST_F(AutoFilterTest, VerifyReflexiveReciept) {
  AutoRequired<FilterA> filterA;
  AutoRequired<FilterC> filterC;
  AutoRequired<FilterD> filterD;
  AutoRequired<AutoPacketFactory> factory;

  AutoCurrentContext()->Initiate();

  // Obtain a packet first:
  auto packet = factory->NewPacket();

  // The mere act of obtaining a packet should have triggered filterD to be fired:
  EXPECT_TRUE(filterD->m_called) << "Trivial filter was not called as expected";

  // Decorate--should satisfy filterC
  packet->Decorate(Decoration<0>());
  EXPECT_TRUE(filterC->m_called) << "FilterC should have been satisfied with one decoration";

  // FilterC should have also satisfied filterA:
  EXPECT_TRUE(filterA->m_called) << "FilterA should have been satisfied by FilterC";
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
    ASSERT_TRUE(makesDec1->m_called) << "Single-input autofilter was not called as expected";

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
      boost::this_thread::sleep_for(boost::chrono::milliseconds(10));

    nReceived++;
    return Deferred(this);
  }

  size_t nReceived;
};

TEST_F(AutoFilterTest, DeferredRecieptInSubContext) {
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

  // Terminate enclosing context
  AutoCurrentContext()->SignalShutdown(true);

  ASSERT_TRUE(sif->hit) << "Simple filter was not invoked by immediate-mode decoration";
  ASSERT_FALSE(dif->hit) << "Deferred filter incorrectly invoked in an immediate mode decoration";
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
  ASSERT_TRUE(fg->m_called) << "Filter not called during multisimultaneous immediate-mode decoration";
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
  ASSERT_TRUE(secondChanceImmed->m_called) << "Filter should have been saturated by an immediate call, but was not called as expected";
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
  ASSERT_TRUE(fg1->m_called) << "Trivial filter was not called as expected, even though Decoration<0> should have been available";
  ASSERT_TRUE(fg2->m_called) << "Filter with an unsatisfied optional argument was not called";
  ASSERT_TRUE(fg3->m_called) << "Saturated filter was not called as expected";
  ASSERT_FALSE(fg4->m_called) << "Undersaturated filter was called even though it should not have been";
}