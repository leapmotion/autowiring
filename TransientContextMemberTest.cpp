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
  {
    s_ctorCount++;
  }

  ~MyTransientClass(void) {
    s_ctorCount--;
  }

  virtual void ZeroArgsA(void) override {
    m_hitCount++;
  }

  virtual Deferred ZeroArgsADeferred(void) override {
    m_hitCount++;
    return Deferred();
  }

  static int s_ctorCount;

  int m_hitCount;
};

int MyTransientClass::s_ctorCount;

typedef TransientPool<MyTransientClass> MyTransientPool;

TransientContextMemberTest::TransientContextMemberTest(void) {
  MyTransientClass::s_ctorCount = 0;
}

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

  // Ensure that just a single element is created:
  EXPECT_EQ(1, MyTransientClass::s_ctorCount) << "An unexpected number of witness elements were created";

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

TEST_F(TransientContextMemberTest, SimplePoolTeardown) {
  {
    AutoCreateContext ctxt;
  
    // Pool creation:
    (CurrentContextPusher)ctxt,
    AutoRequired<MyTransientPool>();
  }

  // Verify that this didn't leak anything:
  EXPECT_EQ(0, MyTransientClass::s_ctorCount) << "A dangling transient instance still exists after context teardown";
}

TEST_F(TransientContextMemberTest, AllTeardown) {
  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);

    // Pool creation:
    AutoRequired<MyTransientPool> pool;

    // Ensure that just a single witness element is created:
    EXPECT_EQ(1, MyTransientClass::s_ctorCount) << "An unexpected number of witness elements were created";
    
    // Now create our transient instance:
    AutoTransient<MyTransientClass> recipient(*pool);

    // Verify that one more instance is created:
    EXPECT_EQ(2, MyTransientClass::s_ctorCount) << "A transient instance was not created as expected";

    // Create a sample sender:
    AutoFired<TransientEvent> sender;
  }

  // Verify that no dangling types remain:
  EXPECT_EQ(0, MyTransientClass::s_ctorCount) << "A dangling transient instance still exists after context teardown";
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