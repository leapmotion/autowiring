#include "stdafx.h"
#include "TransientContextMemberTest.h"
#include "TransientContextMember.h"

class TransientEventA:
  public EventReceiver
{
public:
  virtual void ZeroArgsA(void) = 0;
};

class TransientEventB:
  public EventReceiver
{
public:
  virtual void ZeroArgsB(void) = 0;
};

class TransientClassA:
  public TransientContextMember,
  public TransientEventA,
  public EventSender<TransientEventB>
{
public:
  TransientClassA(void):
    m_received(false)
  {}

  virtual void ZeroArgsA(void) override {
    m_received = true;
  }

  bool m_received;

  using EventSender::Fire;
};

class DurableClassA:
  public EventSender<TransientEventA>,
  public TransientEventB
{
public:
  DurableClassA(void):
    m_received(false)
  {}

  virtual void ZeroArgsB(void) override {
    m_received = true;
  }

  bool m_received;

  using EventSender::Fire;
};

TEST_F(TransientContextMemberTest, VerifyTransience) {
  std::weak_ptr<TransientClassA> transWeak;

  AutoRequired<DurableClassA> durable;
  {
    AutoTransient<TransientClassA> trans;
    transWeak = trans;

    // Verify bidirectional event transmission:
    trans->Fire(&TransientEventB::ZeroArgsB)();
    durable->Fire(&TransientEventA::ZeroArgsA)();

    EXPECT_TRUE(trans->m_received) << "Transient class did not receive an event as expected";
    EXPECT_TRUE(durable->m_received) << "Durable instance did not receive a message from a transient instance";
  }

  EXPECT_TRUE(transWeak.expired()) << "Transient instance did not expire as expected";
}