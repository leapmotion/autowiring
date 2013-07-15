#include "stdafx.h"
#include "TransientContextMemberTest.h"
#include "TransientContextMember.h"
#include "TransientPool.h"

class TransientEvent:
  public virtual EventReceiver
{
public:
  virtual void ZeroArgsA(void) = 0;
  virtual Deferred ZeroArgsADeferred(void) = 0;
};

class MyTransientClass:
  public TransientContextMember,
  public TransientEvent
{
public:
  MyTransientClass(void):
    m_hitCount(0)
  {}

  virtual void ZeroArgsA(void) override {
    m_hitCount++;
  }

  virtual Deferred ZeroArgsADeferred(void) override {
    m_hitCount++;
    return Deferred();
  }

  int m_hitCount;
};

class MyTransientPool:
  public TransientPool<MyTransientClass>
{
};

TEST_F(TransientContextMemberTest, VerifyTransience) {
  // Weak pointer for the transient instance:
  std::weak_ptr<MyTransientClass> transWeak;

  // Generic event sender:
  AutoFired<TransientEvent> durable;
  
  // Pool declaration:
  AutoRequired<MyTransientPool> pool;

  {
    // Create the transient instance and copy over the weak pointer:
    AutoTransient<MyTransientClass> trans(*pool);
    transWeak = trans;

    // Ensure that there are no duplicate shared pointers anywhere:
    EXPECT_TRUE(trans.unique()) << "Transient instance was not unique after construction";

    // Verify the transient instance gets the event as expected:
    durable(&TransientEvent::ZeroArgsA)();
    EXPECT_EQ(trans->m_hitCount, 1) << "Transient class did not receive an event as expected";
  }

  EXPECT_TRUE(transWeak.expired()) << "Transient instance did not expire as expected";

  // Verify nothing explodes if we try to fire an event again:
  EXPECT_NO_THROW(durable(&TransientEvent::ZeroArgsA)()) << "Firing after transient eviction caused an exception";
}

TEST_F(TransientContextMemberTest, VerifyTransientDeferred) {
  // Start up the context:
  AutoCurrentContext ctxt;
  ctxt->InitiateCoreThreads();

  // Pool creation:
  AutoRequired<MyTransientPool> pool;

  // Create the sender and recipient:
  AutoFired<TransientEvent> sender;
  AutoTransient<MyTransientClass> recipient(*pool);

  // Wait for the transient pool to become ready:
  pool->DelayUntilCanAccept();

  // Attempt to defer:
  sender.Defer(&TransientEvent::ZeroArgsADeferred)();

  // Signal the pool to quit and wait until it does:
  pool->Stop(true);
  pool->Wait();

  // Now verify that the receipt count was what we expected:
  EXPECT_EQ(recipient->m_hitCount, 1) << "Deferred call on a transient instance was not received";
}

TEST_F(TransientContextMemberTest, VerifyTransiencePathological) {
  // Pool creation:
  AutoRequired<MyTransientPool> pool;

  // Vector of transient elements:
  std::vector<std::shared_ptr<MyTransientClass>> elements;
}

TEST_F(TransientContextMemberTest, VerifyNoAddToStopped) {
  // Create our pool and immediately stop it:
  AutoRequired<MyTransientPool> pool;
  pool->Stop();

  // This should throw an exception unconditionally:
  std::shared_ptr<MyTransientClass> myTransient;
  EXPECT_THROW(pool->Add(myTransient), std::runtime_error);
}