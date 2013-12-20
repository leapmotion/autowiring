#include "stdafx.h"
#include "PeerContextTest.h"
#include "CoreThread.h"
#include "TestFixtures/SimpleObject.h"
#include "TestFixtures/SimpleReceiver.h"

struct PeerContextName1 {};
struct PeerContextName2 {};

TEST_F(PeerContextTest, VerifySimplePeerage) {
  // Insert a simple receiver first:
  AutoRequired<SimpleReceiver> sr;

  {
    // Create an ordinary child anonymous context and try to fire.  This should
    // not cause anything to be picked up anywhere.
    auto child = m_create->CreateAnonymous();
    CurrentContextPusher pshr(child);
    AutoFired<CallableInterface> ci;
    ci(&CallableInterface::OneArg)(21);
  }
  ASSERT_FALSE(sr->m_one) << "An event originating in a child context incorrectly leaked out into a parent context";

  {
    // Create a single peer context, make it current, and try to fire:
    auto peer = m_create->CreatePeer<PeerContextName1>();
    ASSERT_NE(nullptr, peer.get()) << "Peer context creation method returned a null pointer";

    CurrentContextPusher pshr(peer);
    AutoFired<CallableInterface> ci;
    ci(&CallableInterface::OneArg)(22);
  }

  // Verify that the simple receiver picked up this event, even though it
  // was fired from another context:
  EXPECT_TRUE(sr->m_one) << "Simple reciever did not receive an event originating in a peer context";
  EXPECT_EQ(22, sr->m_oneArg) << "Simple reciever did not recieve the correct argument from a peer context event";
}

TEST_F(PeerContextTest, VerifyPeerTransitivity) {
  // Insert our simple receiver:
  AutoRequired<SimpleReceiver> sr;

  // Now create the first peer context:
  auto peer1 = m_create->CreatePeer<PeerContextName1>();
  ASSERT_NE(nullptr, peer1.get());

  // Create the _second_ peer context based on the first:
  auto peer2 = peer1->CreatePeer<PeerContextName2>();
  ASSERT_NE(nullptr, peer2.get());

  // Fire the event here:
  CurrentContextPusher pshr(peer2);
  AutoFired<CallableInterface> ci;
  ci(&CallableInterface::OneArg)(25);

  // Verify that the event receiver in the base context managed to get the message:
  ASSERT_TRUE(sr->m_one) << "Simple receiver did not properly recieve an event from a transitively peered context";
  ASSERT_EQ(25, sr->m_oneArg) << "Simple receiver properly recieved a transitively peered context's event, but its argument was incorrect";
}

TEST_F(PeerContextTest, VerifyNoAutowiringLeakage) {
  // Insert a simple object in the base context
  AutoRequired<SimpleObject>();

  // Create a peer context and make it current:
  auto peer = m_create->CreatePeer<PeerContextName1>();
  CurrentContextPusher pshr(peer);

  // Verify that, in this peer context, SimpleObject is not visible
  Autowired<SimpleObject> so;
  ASSERT_FALSE(so.IsAutowired()) << "An autowiring request incorrectly resolved against an object allocated in a peer context";
}
