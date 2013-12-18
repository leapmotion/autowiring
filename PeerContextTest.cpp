#include "stdafx.h"
#include "PeerContextTest.h"
#include "CoreThread.h"
#include "TestFixtures/SimpleReceiver.h"

struct PeerContextName {};

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
    auto peer = m_create->CreatePeer<PeerContextName>();
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