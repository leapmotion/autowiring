#include "stdafx.h"
#include "TransientContextMemberTest.h"
#include "TransientContextMember.h"
#include "TransientPool.h"

class TransientEvent:
  public virtual EventReceiver
{
public:
  virtual void ZeroArgsA(void) = 0;
};

class TransientClass:
  public TransientContextMember,
  public TransientEvent
{
public:
  TransientClass(void):
    m_received(false)
  {}

  virtual void ZeroArgsA(void) override {
    m_received = true;
  }

  bool m_received;
};

class DurableClass:
  public EventSender<TransientEvent>
{
public:
  DurableClass(void):
    m_received(false)
  {}

  bool m_received;

  using EventSender::Fire;
};

TEST_F(TransientContextMemberTest, VerifyTransience) {
  // Weak pointer for the transient instance:
  std::weak_ptr<TransientClass> transWeak;

  // Generic event sender:
  AutoRequired<DurableClass> durable;
  
  // Pool type and pool declaration:
  class MyTransientPool:
    public TransientPool<TransientClass>
  {
  };
  AutoRequired<MyTransientPool> pool;

  {
    // Create the transient instance and copy over the weak pointer:
    AutoTransient<TransientClass> trans(*pool);
    transWeak = trans;

    // Ensure that there are no duplicate shared pointers anywhere:
    EXPECT_TRUE(trans.unique()) << "Transient instance was not unique after construction";

    // Verify the transient instance gets the event as expected:
    durable->Fire(&TransientEvent::ZeroArgsA)();
    EXPECT_TRUE(trans->m_received) << "Transient class did not receive an event as expected";
  }

  EXPECT_TRUE(transWeak.expired()) << "Transient instance did not expire as expected";
}