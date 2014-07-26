// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoPacketFactoryTest.hpp"
#include <autowiring/CoreThread.h>

TEST_F(AutoPacketFactoryTest, VerifyNoIssueWhileNotStarted) {
  AutoRequired<AutoPacketFactory> factory;
  ASSERT_THROW(factory->NewPacket(), autowiring_error) << "Issuing a packet in a context that has not yet been started should throw an exception";
}

TEST_F(AutoPacketFactoryTest, StopReallyStops) {
  AutoCurrentContext()->SignalShutdown();

  AutoRequired<AutoPacketFactory> factory;
  ASSERT_TRUE(factory->ShouldStop()) << "Expected that an attempt to insert a packet factory to an already-stopped context would stop the packet factory";
}

TEST_F(AutoPacketFactoryTest, VerifyNoIssueWhileStopped) {
  AutoCurrentContext()->SignalShutdown();

  AutoRequired<AutoPacketFactory> factory;
  ASSERT_THROW(factory->NewPacket(), autowiring_error) << "Issuing a packet in a context that has already been stopped should throw an exception";
}

class IssuesPacketWaitsThenQuits:
  public CoreThread
{
public:
  IssuesPacketWaitsThenQuits(void) :
    m_hasQuit(false)
  {
    // Note:  Don't do this in practice.  This only works because we only inject this type
    // into a context that's already running; normally, creating a packet from our ctor can
    // cause an exception if we are being injected before Initiate is called.
    AutoRequired<AutoPacketFactory> factory;
    m_packet = factory->NewPacket();
  }

  bool m_hasQuit;
  std::shared_ptr<AutoPacket> m_packet;

  void Run(void) override {
    // Move shared pointer locally:
    std::shared_ptr<AutoPacket> packet;
    std::swap(packet, m_packet);

    // Just wait a bit, then return, just like we said we would
    this->ThreadSleep(std::chrono::milliseconds(50));

    // Update our variable and then return out:
    m_hasQuit = true;
  }
};


TEST_F(AutoPacketFactoryTest, WaitRunsDownAllPackets) {
  AutoCurrentContext()->Initiate();

  // Create a factory in our context, factory had better be started:
  AutoRequired<AutoPacketFactory> factory;
  ASSERT_TRUE(factory->IsRunning()) << "Factory was not started even though it was a member of an initiated context";

  // Make the thread create and hold a packet, and then return
  AutoRequired<IssuesPacketWaitsThenQuits> ipwtq;
  
  // Shutdown context
  AutoCurrentContext()->SignalShutdown();

  // Now we're going to try to run down the factory:
  factory->Wait();

  // Verify that the thread has quit:
  ASSERT_TRUE(ipwtq->m_hasQuit) << "AutoPacketFactory::Wait returned prematurely";
}

class HoldsAutoPacketFactoryReference {
public:
  HoldsAutoPacketFactoryReference(void):
    m_value(0)
  {}

  AutoRequired<AutoPacketFactory> m_factory;
  int m_value;

  // Just a dummy AutoFilter method so that this class is recognized as an AutoFilter
  void AutoFilter(int value) {
    m_value = value;
  }
};

TEST_F(AutoPacketFactoryTest, AutoPacketFactoryCycle) {
  std::weak_ptr<CoreContext> ctxtWeak;
  std::weak_ptr<HoldsAutoPacketFactoryReference> hapfrWeak;
  std::shared_ptr<AutoPacket> packet;

  {
    // Create a context, fill it up, kick it off:
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);
    AutoRequired<HoldsAutoPacketFactoryReference> hapfr(ctxt);
    ctxt->Initiate();
    
    // A weak pointer is used to detect object destruction
    ctxtWeak = ctxt;
    hapfrWeak = hapfr;

    // Trivial validation-of-reciept:
    AutoRequired<AutoPacketFactory> factory;
    {
      auto trivial = factory->NewPacket();
      trivial->Decorate((int) 54);
      ASSERT_EQ(54, hapfr->m_value) << "A simple packet was not received as expected by an AutoFilter";
    }

    // Create a packet which will force in a back-reference:
    packet = factory->NewPacket();

    // Terminate the context:
    ctxt->SignalShutdown();

    // Verify that we can still decorate the packet and also that the packet is delivered to the factory:
    packet->Decorate((int) 55);

    // Relock, verify the value was received by the hapfr:
    ASSERT_EQ(55, hapfr->m_value) << "AutoFilter did not receive a packet as expected";
  }

  // Verify that the context went out of scope as expected
  ASSERT_TRUE(ctxtWeak.expired()) << "AutoPacketFactory incorrectly held a cyclic reference even after the context was shut down";

  // Now we can release the packet and verify that everything gets cleaned up:
  packet.reset();
  ASSERT_TRUE(hapfrWeak.expired()) << "The last packet from a factory was released; this should have resulted in teardown, but it did not";
}