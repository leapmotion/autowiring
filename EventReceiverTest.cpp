#include "stdafx.h"
#include "EventReceiverTest.h"
#include "Autowiring/Autowired.h"
#include "Autowiring/CoreThread.h"

class CallableInterface:
  public EventReceiver
{
public:
  virtual void ZeroArgs(void) = 0;
  virtual void OneArg(int arg) = 0;
};

class SimpleSender:
  public EventManager<CallableInterface>
{
};

class SimpleReceiver:
  public CoreThread,
  public CallableInterface
{
public:
  SimpleReceiver(void):
    m_zero(false),
    m_one(false),
    m_oneArg(0)
  {
    Ready();
  }

  // Manifest of functions called:
  bool m_zero;

  bool m_one;
  int m_oneArg;

  virtual void ZeroArgs(void) override {
    m_zero = true;
  }

  virtual void OneArg(int arg) override {
    m_one = true;
    m_oneArg = arg;
  }
};

TEST_F(EventReceiverTest, SimpleMethodCall) {
  AutoCreateContext ctxt;
  CurrentContextPusher pshr(ctxt);

  AutoRequired<SimpleReceiver> receiver;
  AutoRequired<SimpleSender> sender;

  // Start up the context:
  ctxt->InitiateCoreThreads();

  // Try firing the event first:
  sender->Fire(&CallableInterface::ZeroArgs)();
  sender->Fire(&CallableInterface::OneArg)(100);

  // Now we verify the event was received by our receiver:
  EXPECT_TRUE(receiver->m_zero);
  EXPECT_TRUE(receiver->m_one);
  EXPECT_EQ(100, receiver->m_oneArg);

  // Cleanup time
  ctxt->SignalShutdown();
  ctxt->Wait();
}