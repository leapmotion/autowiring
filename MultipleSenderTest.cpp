#include "stdafx.h"
#include "MultipleSenderTest.h"
#include "EventSender.h"

class MultiEventA:
  public virtual EventReceiver
{
public:
  virtual void EventA(void) = 0;
};

class MultiEventB:
  public virtual EventReceiver
{
public:
  virtual void EventB(void) = 0;
};

class MultiSender:
  public EventSender<MultiEventA, MultiEventB>
{
};

MultipleSenderTest::MultipleSenderTest(void) {}

TEST_F(MultipleSenderTest, VerifyMultiSendA) {
  m_sender->Fire(&MultiEventA::EventA)();
}

TEST_F(MultipleSenderTest, VerifyMultiSendB) {
  m_sender->Fire(&MultiEventB::EventB)();
}
