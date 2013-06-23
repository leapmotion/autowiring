#include "stdafx.h"
#include "MultipleSenderTest.h"
#include "EventSender.h"

class MultiEventA:
  public virtual EventReceiver
{
public:
  virtual void Event(void) = 0;
};

class MultiEventB:
  public virtual EventReceiver
{
public:
  virtual void Event(void) = 0;
};

class MultiSender:
  public EventSender<MultiEventA, MultiEventB>
{
};

MultipleSenderTest::MultipleSenderTest(void) {}

TEST_F(MultipleSenderTest, VerifyMultiSendA) {
  m_sender->Fire(&MultiEventA::Event)();
}

TEST_F(MultipleSenderTest, VerifyMultiSendB) {
  m_sender->Fire(&MultiEventB::Event)();
}
