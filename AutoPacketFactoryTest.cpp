#include "stdafx.h"
#include "AutoPacketFactoryTest.h"
#include "CoreThread.h"

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
    // Move the shared pointer here:
    auto packet = std::move(m_packet);

    // Just wait a bit, then return, just like we said we would
    this->ThreadSleep(boost::chrono::milliseconds(50));

    // Update our variable and then return out:
    m_hasQuit = true;
  }
};

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

TEST_F(AutoPacketFactoryTest, WaitRunsDownAllPackets) {
  AutoCurrentContext()->Initiate();

  // Create a factory in our context, factory had better be started:
  AutoRequired<AutoPacketFactory> factory;
  ASSERT_TRUE(factory->IsRunning()) << "Factory was not started even though it was a member of an initiated context";

  // Make the thread create and hold a packet, and then return
  AutoRequired<IssuesPacketWaitsThenQuits> ipwtq;

  // Now we're going to try to run down the factory:
  factory->Wait();

  // Verify that the thread has quit:
  ASSERT_TRUE(ipwtq->m_hasQuit) << "AutoPacketFactory::Wait returned prematurely";
}