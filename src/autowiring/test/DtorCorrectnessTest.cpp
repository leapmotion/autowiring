// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/CoreThread.h>
#include ATOMIC_HEADER

using namespace std;

class CtorDtorListener;
template<int i>
class MyCtorDtorListenerN;

class DtorCorrectnessTest :
  public testing::Test
{
public:
  DtorCorrectnessTest(void);

  void SetUp(void) override;

  AutoRequired<MyCtorDtorListenerN<1>> listener1;
  AutoRequired<MyCtorDtorListenerN<2>> listener2;
};

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

class MyCtorDtorListener:
  public CoreThread
{
public:
  MyCtorDtorListener(void):
    CoreThread("MyCtorDtorListener")
  {}

  bool m_hitDeferred = false;

  void DoDeferred(CtorDtorCopyCounter ctr) {
    *this += [this] {
      m_hitDeferred = true;
    };
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
  listener1->DoDeferred({});
  listener2->DoDeferred({});

  // Process all deferred elements and then check to see what we got:
  AutoCurrentContext ctxt;
  ctxt->Initiate();
  listener1->Stop(true);
  listener2->Stop(true);
  listener1->Wait();
  listener2->Wait();

  // Verify that we actually hit something:
  ASSERT_TRUE(listener1->m_hitDeferred) << "Failed to hit listener #1's deferred call";
  ASSERT_TRUE(listener2->m_hitDeferred) << "Failed to hit listener #2's deferred call";

  // Release all of our pointers:
  listener1.reset();
  listener2.reset();

  // Verify hit counts:
  ASSERT_LE(2UL, CtorDtorCopyCounter::s_construction) << "Counter constructors were not invoked the expected number of times when deferred";
  ASSERT_EQ(0, CtorDtorCopyCounter::s_outstanding) << "Counter mismatch under deferred events";
}
