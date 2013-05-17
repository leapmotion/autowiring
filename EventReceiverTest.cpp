#include "stdafx.h"
#include "EventReceiverTest.h"
#include "Autowiring/CoreThread.h"

class CallableInterface {
public:
  virtual void ZeroArgs(void) = 0;
  virtual void OneArg(int arg);
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

  // Our primary event loop:
  void Run(void) override {

  }
};

TEST_F(EventReceiverTest, SimpleMethodCall) {
}