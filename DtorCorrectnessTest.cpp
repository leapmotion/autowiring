#include "stdafx.h"
#include "DtorCorrectnessTest.h"

class CtorDtorCopyCounter {
public:
  static int s_count;
  static size_t s_hit;

  CtorDtorCopyCounter(void) {
    s_count++;
    s_hit++;
  }

  CtorDtorCopyCounter(const CtorDtorCopyCounter&) {
    s_count++;
    s_hit++;
  }

  ~CtorDtorCopyCounter(void) {
    s_count--;
  }
};

int CtorDtorCopyCounter::s_count = 0;
size_t CtorDtorCopyCounter::s_hit = 0;

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
    m_hitDeferred(false)
  {
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

class MyCtorDtorListener1:
  public MyCtorDtorListener
{};

class MyCtorDtorListener2:
  public MyCtorDtorListener
{};

DtorCorrectnessTest::DtorCorrectnessTest(void) {
}

void DtorCorrectnessTest::SetUp(void) {
  // Reset counter:
  CtorDtorCopyCounter::s_count = 0;
  CtorDtorCopyCounter::s_hit = 0;
}

TEST_F(DtorCorrectnessTest, VerifyFiringDtors) {
  // Try firing some events and validate the invariant:
  cdl(&CtorDtorListener::DoFired)(CtorDtorCopyCounter());
  EXPECT_LE(2UL, CtorDtorCopyCounter::s_hit) << "Counter constructors were not invoked the expected number of times when fired";
  EXPECT_EQ(0, CtorDtorCopyCounter::s_count) << "Counter mismatch under event firing";
}

#if __APPLE__
TEST_F(DtorCorrectnessTest, DISABLED_VerifyDeferringDtors)
#else
TEST_F(DtorCorrectnessTest, VerifyDeferringDtors)
#endif
{
  // Make sure our threads are running:
  AutoCurrentContext ctxt;
  ctxt->InitiateCoreThreads();

  // Spin until both threads are ready to accept:
  listener1->DelayUntilCanAccept();
  listener2->DelayUntilCanAccept();

  // Now try deferring:
  cdl(&CtorDtorListener::DoDeferred)(CtorDtorCopyCounter());
  listener1->Stop(true);
  listener2->Stop(true);
  listener1->Wait();
  listener2->Wait();

  // Verify that we actually hit something:
  EXPECT_TRUE(listener1->m_hitDeferred) << "Failed to hit a listener's deferred call";
  EXPECT_TRUE(listener2->m_hitDeferred) << "Failed to hit a listener's deferred call";

  // Verify hit counts:
  EXPECT_LE(2UL, CtorDtorCopyCounter::s_hit) << "Counter constructors were not invoked the expected number of times when deferred";
  EXPECT_EQ(0, CtorDtorCopyCounter::s_count) << "Counter mismatch under deferred events";
}
