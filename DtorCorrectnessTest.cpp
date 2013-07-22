#include "stdafx.h"
#include "DtorCorrectnessTest.h"
#include <iostream>

using namespace std;

class CtorDtorCopyCounter {
public:
  static int s_outstanding;
  static size_t s_construction;

  CtorDtorCopyCounter(void) {
    s_outstanding++;
    s_construction++;
  }

  CtorDtorCopyCounter(const CtorDtorCopyCounter&) {
    s_outstanding++;
    s_construction++;
  }

  ~CtorDtorCopyCounter(void) {
    s_outstanding--;
  }
};

int CtorDtorCopyCounter::s_outstanding = 0;
size_t CtorDtorCopyCounter::s_construction = 0;

class CtorDtorListener:
  public virtual EventReceiver
{
public:
  virtual void DoFired(CtorDtorCopyCounter ctr) = 0;
  virtual Deferred DoDeferred(CtorDtorCopyCounter ctr) = 0;
};

class MyCtorDtorListener:
  public CoreThread,
  public CtorDtorListener
{
public:
  MyCtorDtorListener(void):
    CoreThread("MyCtorDtorListener"),
    m_hitDeferred(false)
  {
    AcceptDispatchDelivery();
    Ready();
  }

  bool m_hitDeferred;

  virtual void DoFired(CtorDtorCopyCounter ctr) {
  }

  virtual Deferred DoDeferred(CtorDtorCopyCounter ctr) {
    m_hitDeferred = true;
    return Deferred(this);
  }
};

template<int i>
class MyCtorDtorListenerN:
  public MyCtorDtorListener
{
public:
  MyCtorDtorListenerN(void)
  {}
};

DtorCorrectnessTest::DtorCorrectnessTest(void) {
}

void DtorCorrectnessTest::SetUp(void) {
  // Reset counter:
  CtorDtorCopyCounter::s_outstanding = 0;
  CtorDtorCopyCounter::s_construction = 0;
}

TEST_F(DtorCorrectnessTest, VerifyFiringDtors) {
  // Try firing some events and validate the invariant:
  cdl(&CtorDtorListener::DoFired)(CtorDtorCopyCounter());
  EXPECT_LE(2UL, CtorDtorCopyCounter::s_construction) << "Counter constructors were not invoked the expected number of times when fired";
  EXPECT_EQ(0, CtorDtorCopyCounter::s_outstanding) << "Counter mismatch under event firing";
}

TEST_F(DtorCorrectnessTest, VerifyDeferringDtors) {
  // Precondition verification:
  ASSERT_FALSE(listener1->ShouldStop()) << "Thread was signalled to stop before it even started";

  // Simple check of our copy counter under scope:
  {
    CtorDtorCopyCounter tempCounter;
    ASSERT_EQ(1UL, CtorDtorCopyCounter::s_construction) << "Constructor count was not incremented correctly";
    CtorDtorCopyCounter::s_construction = 0;
  }
  ASSERT_EQ(0UL, CtorDtorCopyCounter::s_outstanding) << "Unexpected number of outstanding instances";

  // Simple check of anonymous instance handling:
  CtorDtorCopyCounter();
  ASSERT_EQ(1UL, CtorDtorCopyCounter::s_construction) << "Constructor count was not incremented correctly";
  CtorDtorCopyCounter::s_construction = 0;
  ASSERT_EQ(0UL, CtorDtorCopyCounter::s_outstanding) << "Unexpected number of outstanding instances";

  // Verify that the thread didn't exit too soon:
  ASSERT_FALSE(listener1->ShouldStop()) << "Thread was signalled to stop even though it should have been deferring";

  // Spin until both threads are ready to accept:
  ASSERT_TRUE(listener1->DelayUntilCanAccept()) << "First listener reported it could not accept";
  ASSERT_TRUE(listener2->DelayUntilCanAccept()) << "Second listener reported it could not accept";

  // Now try deferring:
  cdl(&CtorDtorListener::DoDeferred)(CtorDtorCopyCounter());

  // Process all deferred elements and then check to see what we got:
  AutoCurrentContext ctxt;
  ctxt->InitiateCoreThreads();
  listener1->Stop(true);
  listener2->Stop(true);
  listener1->Wait();
  listener2->Wait();

  // Verify that we actually hit something:
  EXPECT_TRUE(listener1->m_hitDeferred) << "Failed to hit a listener's deferred call";
  EXPECT_TRUE(listener2->m_hitDeferred) << "Failed to hit a listener's deferred call";

  // Release all of our pointers:
  m_create.reset();
  listener1.reset();
  listener2.reset();

  // Verify hit counts:
  //EXPECT_LE(2UL, CtorDtorCopyCounter::s_construction) << "Counter constructors were not invoked the expected number of times when deferred";
  EXPECT_EQ(0, CtorDtorCopyCounter::s_outstanding) << "Counter mismatch under deferred events";
}