// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "DtorCorrectnessTest.h"
#include "CoreThread.h"
#include ATOMIC_HEADER

using namespace std;

class CtorDtorCopyCounter {
public:
  static std::atomic<int> s_outstanding;
  static std::atomic<size_t> s_construction;

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

std::atomic<int> CtorDtorCopyCounter::s_outstanding;
std::atomic<size_t> CtorDtorCopyCounter::s_construction;

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
  {}

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

void convert(int x);

TEST_F(DtorCorrectnessTest, VerifyFiringDtors) {
  AutoCurrentContext()->Initiate();

  // Try firing some events and validate the invariant:
  cdl(&CtorDtorListener::DoFired)(CtorDtorCopyCounter());
  EXPECT_LE(2UL, CtorDtorCopyCounter::s_construction) << "Counter constructors were not invoked the expected number of times when fired";
  EXPECT_EQ(0, CtorDtorCopyCounter::s_outstanding) << "Counter mismatch under event firing";
}

TEST_F(DtorCorrectnessTest, VerifyDeferringDtors) {
  AutoCurrentContext()->Initiate();
  {
    CtorDtorCopyCounter tempCounter;
    ASSERT_EQ(1UL, CtorDtorCopyCounter::s_construction) << "Constructor count was not incremented correctly";
    CtorDtorCopyCounter::s_construction = 0;
  }
  ASSERT_EQ(0, CtorDtorCopyCounter::s_outstanding) << "Unexpected number of outstanding instances";

  // Simple check of anonymous instance handling:
  CtorDtorCopyCounter();
  ASSERT_EQ(1UL, CtorDtorCopyCounter::s_construction) << "Constructor count was not incremented correctly";
  CtorDtorCopyCounter::s_construction = 0;
  ASSERT_EQ(0, CtorDtorCopyCounter::s_outstanding) << "Unexpected number of outstanding instances";

  // Verify that the thread didn't exit too soon:
  ASSERT_FALSE(listener1->ShouldStop()) << "Thread was signalled to stop even though it should have been deferring";

  // Now try deferring:
  cdl(&CtorDtorListener::DoDeferred)(CtorDtorCopyCounter());

  // Process all deferred elements and then check to see what we got:
  AutoCurrentContext ctxt;
  ctxt->Initiate();
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
  EXPECT_LE(2UL, CtorDtorCopyCounter::s_construction) << "Counter constructors were not invoked the expected number of times when deferred";
  EXPECT_EQ(0, CtorDtorCopyCounter::s_outstanding) << "Counter mismatch under deferred events";
}
