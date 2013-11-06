#include "stdafx.h"
#include "DecoratorTest.h"
#include "AutoPacket.h"
#include "AutoPacketFactory.h"
#include "AutoPacketListener.h"
#include "FilterPropertyExtractor.h"
#include "TestFixtures/Decoration.h"
#include <boost/thread/barrier.hpp>

using namespace std;

class FilterRoot {
public:
  FilterRoot(void) :
    m_called(false)
  {}

  bool m_called;
  Decoration<0> m_zero;
  Decoration<1> m_one;
};

class FilterA:
  public FilterRoot
{
public:
  void AutoFilter(Decoration<0> zero, Decoration<1> one) {
    m_called = true;
    m_zero = zero;
    m_one = one;
  }
};
static_assert(has_autofilter<FilterA>::value, "Expected the filter to have an AutoFilter method");

class FilterB:
  public CoreThread,
  public FilterRoot
{
public:
  FilterB(void) :
    m_barr(2)
  {
    // We'll accept dispatch delivery as long as we exist:
    AcceptDispatchDelivery();
    Ready();
  }

  Deferred AutoFilter(Decoration<0> zero, Decoration<1> one) {
    m_called = true;
    m_zero = zero;
    m_one = one;
    return Deferred(this);
  }

  boost::barrier m_barr;

  void Run(void) override {
    m_barr.wait();
    CoreThread::Run();
  }
};

/// <summary>
/// This is a filter which, in addition to accepting a decoration, also accepts a packet and tries to decorate it
/// </summary>
class FilterC:
  public FilterRoot
{
public:
  void AutoFilter(AutoPacket& pkt, const Decoration<0>& zero) {
    // Copy out:
    m_called = true;
    m_zero = zero;

    // Add a decoration:
    pkt.Decorate(Decoration<1>());
  }
};

/// <summary>
/// A filter which will simply get hit any time a packet is issued in the current context
/// </summary>
class FilterD:
  public FilterRoot
{
public:
  void AutoFilter(AutoPacket& pkt) {
    m_called = true;
  }
};

class FilterE:
  public FilterRoot,
  public AutoPacketListener
{
public:
  void OnPacketReturned(const AutoPacket& packet) {
    m_called = true;
  }
};

TEST_F(DecoratorTest, VerifyCorrectExtraction) {
  vector<const type_info*> v;

  // Run our prop extractor based on a known decorator:
  RecipientPropertyExtractor<FilterA>::Enumerate(
    [&v] (const std::type_info& ti) {
      v.push_back(&ti);
    }
  );
  ASSERT_EQ(2UL, v.size()) << "Extracted an insufficient number of types from a known filter function";

  // Arguments MUST be in order:
  EXPECT_EQ(typeid(Decoration<0>), *v[0]);
  EXPECT_EQ(typeid(Decoration<1>), *v[1]);

  // Verify both overloads wind up returning the same array:
  auto ppCur = RecipientPropertyExtractor<FilterA>::Enumerate();
  for(size_t i = 0; ppCur[i]; i++)
    EXPECT_EQ(*v[i], *ppCur[i]) << "Two overloads of Enumerate returned contradictory types";
}

TEST_F(DecoratorTest, VerifyEmptyExtraction) {
  const type_info*const* v = RecipientPropertyExtractor<Object>::Enumerate();
  EXPECT_EQ(nullptr, *v) << "Extracted arguments from an object known not to have a Filter method";
}

TEST_F(DecoratorTest, VerifySimplePacketDecoration) {
  AutoRequired<AutoPacketFactory> factory;

  // Create the packet we will be persisting:
  auto packet = factory->NewPacket();

  // Add a few decorations on this packet:
  auto& knownDec0 = packet->Decorate(Decoration<0>());
  auto& knownDec1 = packet->Decorate(Decoration<1>());
  auto& knownDec2 = packet->Decorate(Decoration<2>());

  // Verify we can get these packets back--might throw exceptions here!
  auto& dec0 = packet->Get<Decoration<0>>();
  auto& dec1 = packet->Get<Decoration<1>>();
  auto& dec2 = packet->Get<Decoration<2>>();

  // Verify identities:
  EXPECT_EQ(&knownDec0, &dec0) << "Decoration 0 returned at an incorrect location";
  EXPECT_EQ(&knownDec1, &dec1) << "Decoration 1 returned at an incorrect location";
  EXPECT_EQ(&knownDec2, &dec2) << "Decoration 2 returned at an incorrect location";

  // Verify content correctness:
  EXPECT_EQ(0, dec0.i) << "Decoration 0 incorrectly persisted";
  EXPECT_EQ(1, dec1.i) << "Decoration 1 incorrectly persisted";
  EXPECT_EQ(2, dec2.i) << "Decoration 2 incorrectly persisted";
}

TEST_F(DecoratorTest, VerifyDecoratorAwareness) {
  // Create a packet while the factory has no subscribers:
  AutoRequired<AutoPacketFactory> factory;
  auto packet1 = factory->NewPacket();

  // Verify subscription-free status:
  EXPECT_FALSE(packet1->HasSubscribers<Decoration<0>>()) << "Subscription exists where one should not have existed";

  // Create another packet where a subscriber exists:
  AutoRequired<FilterA> filterA;
  ASSERT_TRUE(factory->IsSubscriber<FilterA>());
  auto packet2 = factory->NewPacket();
  
  // Verify the first packet still does not have subscriptions:
  EXPECT_FALSE(packet1->HasSubscribers<Decoration<0>>()) << "Subscription was incorrectly, retroactively added to a packet";

  // Verify the second one does:
  EXPECT_TRUE(packet2->HasSubscribers<Decoration<0>>()) << "Packet lacked an expected subscription";
}

TEST_F(DecoratorTest, VerifySimpleFilter) {
  AutoRequired<FilterA> filterA;
  AutoRequired<AutoPacketFactory> factory;

  // Manually register the subscriber:
  ASSERT_TRUE(factory->IsSubscriber<FilterA>());

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

TEST_F(DecoratorTest, VerifyNoMultiDecorate) {
  AutoRequired<FilterA> filterA;
  AutoRequired<AutoPacketFactory> factory;

  // Subscriber registration verification:
  ASSERT_TRUE(factory->IsSubscriber<FilterA>());

  // Obtain a packet and attempt redundant introduction:
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>());
  EXPECT_THROW(packet->Decorate(Decoration<0>()), std::runtime_error) << "Redundant decoration did not throw an exception as expected";

  // Verify that a call has not yet been made
  EXPECT_FALSE(filterA->m_called) << "A call made on an idempotent packet decoration";

  // Now finish saturating the filter and ensure we get a call:
  packet->Decorate(Decoration<1>());
  EXPECT_TRUE(filterA->m_called) << "Filter was not called after being fully satisfied";
}

TEST_F(DecoratorTest, VerifyInterThreadDecoration) {
  AutoRequired<FilterB> filterB;
  AutoRequired<AutoPacketFactory> factory;
  AutoCurrentContext ctxt;

  // Kick off all threads:
  ctxt->InitiateCoreThreads();

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

TEST_F(DecoratorTest, VerifyTeardownArrangement) {
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

    // Satisfy the packet:
    packet->Decorate(Decoration<0>());
    packet->Decorate(Decoration<1>());
  }

  // Filter should be expired now:
  ASSERT_TRUE(filterAWeak.expired()) << "Subscriber was still left outstanding even though all references should be gone";
}

TEST_F(DecoratorTest, VerifyCheckout) {
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
    AutoPacket::AutoCheckout<Decoration<0>> exterior;

    {
      AutoPacket::AutoCheckout<Decoration<0>> checkout = packet->Checkout<Decoration<0>>();

      // Verify we can move the original type:
      AutoPacket::AutoCheckout<Decoration<0>> checkoutMoved(std::move(checkout));

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

TEST_F(DecoratorTest, RollbackCorrectness) {
  AutoRequired<FilterA> filterA;
  AutoRequired<AutoPacketFactory> factory;

  // Obtain a packet for use with deferred decoration:
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<1>());

  // Request and immediately allow the destruction of a checkout:
  packet->Checkout<Decoration<0>>();

  // Verify no hit took place--the checkout should have been cancelled:
  EXPECT_FALSE(filterA->m_called) << "Filter was not called after all decorations were installed";

  // We should not be able to obtain another checkout of this decoration on this packet:
  EXPECT_ANY_THROW(packet->Checkout<Decoration<0>>()) << "An attempt to check out a decoration a second time should have failed";
  
  // We shouldn't be able to manually decorate, either:
  EXPECT_ANY_THROW(packet->Decorate(Decoration<0>())) << "An attempt to manually add a previously failed decoration succeeded where it should not have";
}

TEST_F(DecoratorTest, VerifyReflexiveReciept) {
  AutoRequired<FilterA> filterA;
  AutoRequired<FilterC> filterC;
  AutoRequired<FilterD> filterD;
  AutoRequired<FilterE> filterE;
  AutoRequired<AutoPacketFactory> factory;

  // Obtain a packet first:
  auto packet = factory->NewPacket();

  // The mere act of obtaining a packet should have triggered filterD to be fired:
  EXPECT_TRUE(filterD->m_called) << "Trivial filter was not called as expected";

  // The packet should be able to obtain a pointer to itself:
  {
    AutoPacket* reflex;
    EXPECT_TRUE(packet->Get(reflex)) << "Packet was unable to obtain a self-reference via Get";
    EXPECT_EQ(packet.get(), reflex) << "Packet reflexive reference was not an identity";

    const AutoPacket* cReflex;
    EXPECT_TRUE(const_cast<const AutoPacket*>(packet.get())->Get(cReflex)) << "Packet was unable to obtain a self-reference via const Get";
    EXPECT_EQ(packet.get(), cReflex) << "Packet reflexive reference was not an identity";
  }

  // Decorate--should satisfy filterC
  packet->Decorate(Decoration<0>());
  EXPECT_TRUE(filterC->m_called) << "FilterC should have been satisfied with one decoration";

  // FilterC should have also satisfied filterA:
  EXPECT_TRUE(filterA->m_called) << "FilterA should have been satisfied by FilterC";

  // Release the packet, and verify that filterD gets hit only once this happens
  EXPECT_FALSE(filterE->m_called) << "Packet listener was notified prematurely";
  packet.reset();
  EXPECT_TRUE(filterE->m_called) << "Packet listener was not notified as anticipated";
}