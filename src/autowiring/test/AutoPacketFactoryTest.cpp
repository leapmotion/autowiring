// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/CoreThread.h>
#include CHRONO_HEADER
#include THREAD_HEADER

class AutoPacketFactoryTest:
  public testing::Test
{};

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
  IssuesPacketWaitsThenQuits(void) {
    // Note:  Don't do this in practice.  This only works because we only inject this type
    // into a context that's already running; normally, creating a packet from our ctor can
    // cause an exception if we are being injected before Initiate is called.
    AutoRequired<AutoPacketFactory> factory;
    m_packet = factory->NewPacket();
  }

  bool m_hasQuit = false;
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
  AutoRequired<AutoPacketFactory> m_factory;
  int m_value = 0;

  // Just a dummy AutoFilter method so that this class is recognized as an AutoFilter
  void AutoFilter(int value) {
    m_value = value;
  }
};

TEST_F(AutoPacketFactoryTest, AutoPacketFactoryCycle) {
  AutoCurrentContext()->Initiate();

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

  // The context cannot go out of socpe until all packets in the context are out of scope
  ASSERT_FALSE(ctxtWeak.expired()) << "Context went out of scope before all packets were finished being processed";

  // Now we can release the packet and verify that everything gets cleaned up:
  packet.reset();
  ASSERT_TRUE(ctxtWeak.expired()) << "AutoPacketFactory incorrectly held a cyclic reference even after the context was shut down";
  ASSERT_TRUE(hapfrWeak.expired()) << "The last packet from a factory was released; this should have resulted in teardown, but it did not";
}

class DelaysAutoPacketsOneMS {
public:
  void AutoFilter(int value) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
};

TEST_F(AutoPacketFactoryTest, AutoPacketStatistics) {
  // Create a context, fill it up, kick it off:
  AutoCurrentContext ctxt;
  AutoRequired<DelaysAutoPacketsOneMS> dapoms;
  AutoRequired<AutoPacketFactory> factory;
  ctxt->Initiate();

  int numPackets = 20;

  // Send 20 packets which should all be delayed 1ms
  for (int i = 0; i < numPackets; ++i) {
    auto packet = factory->NewPacket();
    packet->Decorate(i);
  }

  // Shutdown our context, and rundown our factory
  ctxt->SignalShutdown();
  factory->Wait();

  // Ensure that the statistics are not too wrong
  // We delayed each packet by one ms, and our statistics are given in nanoseconds
  double packetDelay = (double) std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::milliseconds(1)).count();
  ASSERT_EQ(numPackets, factory->GetTotalPacketCount()) << "The factory did not get enough packets";

  ASSERT_LE(packetDelay, factory->GetMeanPacketLifetime()) << "The mean packet lifetime was less than the delay on each packet";
}

TEST_F(AutoPacketFactoryTest, MultipleInstanceAddition) {
  AutoCurrentContext ctxt;
  AutoRequired<AutoPacketFactory> factory;
  ctxt->Initiate();

  bool ary[2] = {};
  for (size_t i = 0; i < 2; i++)
    *factory += [i, &ary] (int) {
      ary[i] = true;
    };

  auto packet = factory->NewPacket();
  packet->Decorate(101);
  ASSERT_TRUE(ary[0]) << "First of two identically typed AutoFilter lambdas was not called";
  ASSERT_TRUE(ary[1]) << "Second of two identically typed AutoFilter lambdas was not called";
}

TEST_F(AutoPacketFactoryTest, AddSubscriberTest) {
  AutoCurrentContext ctxt;
  AutoRequired<AutoPacketFactory> factory;
  ctxt->Initiate();

  bool first_called = false;
  bool second_called = false;

  factory->AddSubscriber(AutoFilterDescriptor([&first_called](int) {first_called = true; }));
  {
    std::vector<AutoFilterDescriptor> descs;
    factory->AppendAutoFiltersTo(descs);
    ASSERT_EQ(1UL, descs.size()) << "Expected exactly one AutoFilters after call to AddSubscriber";
  }

  *factory += [&second_called] (int v) {
    second_called = true;
    ASSERT_EQ(101, v) << "Decoration value mismatch";
  };
  {
    std::vector<AutoFilterDescriptor> descs;
    factory->AppendAutoFiltersTo(descs);
    ASSERT_EQ(2UL, descs.size()) << "Expected exactly two AutoFilters on this packet";
  }

  auto packet = factory->NewPacket();

  ASSERT_FALSE(first_called) << "Normal subscriber called too early";
  ASSERT_FALSE(second_called) << "Subscriber added with operator+= called too early";

  packet->DecorateImmediate(int(101));

  ASSERT_TRUE(first_called) << "Normal subscriber never called";
  ASSERT_TRUE(second_called) << "Subscriber added with operator+= never called";
}

TEST_F(AutoPacketFactoryTest, CanRemoveAddedLambda) {
  AutoCurrentContext()->Initiate();
  AutoRequired<AutoPacketFactory> factory;

  auto desc = *factory += [](int&){};
  auto packet1 = factory->NewPacket();
  *factory -= desc;
  auto packet2 = factory->NewPacket();

  ASSERT_TRUE(packet1->Has<int>()) << "First packet did not posess expected decoration";
  ASSERT_FALSE(packet2->Has<int>()) << "Decoration present even after all filters were removed from a factory";
}

TEST_F(AutoPacketFactoryTest, CurrentPacket) {
  AutoCurrentContext()->Initiate();
  AutoRequired<AutoPacketFactory> factory;
  ASSERT_EQ(nullptr, factory->CurrentPacket()) << "Current packet returned before any packets were issued";
  auto packet = factory->NewPacket();
  ASSERT_EQ(packet, factory->CurrentPacket()) << "Current packet was not reported correctly as being issued to the known current packet";
  packet.reset();
  ASSERT_EQ(nullptr, factory->CurrentPacket()) << "A current packet was reported after the current packet has expired";
}

TEST_F(AutoPacketFactoryTest, IsRunningWhilePacketIssued) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();
  AutoRequired<AutoPacketFactory> factory;
  auto packet = factory->NewPacket();
  ctxt->SignalShutdown();
  ASSERT_TRUE(factory->IsRunning()) << "Factory should be considered to be running as long as packets are outstanding";
}
