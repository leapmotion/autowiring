#include "DtorCorrectnessTest.h"

class CtorDtorCopyCounter {
public:
  static int s_count;

  CtorDtorCopyCounter(void) {
    s_count++;
  }

  CtorDtorCopyCounter(CtorDtorCopyCounter&) {
    s_count++;
  }

  ~CtorDtorCopyCounter(void) {
    s_count--;
  }
};

int CtorDtorCopyCounter::s_count = 0;

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
  virtual void DoFired(CtorDtorCopyCounter ctr) {
  }

  virtual Deferred DoDeferred(CtorDtorCopyCounter ctr) {
    return Deferred(this);
  }
};

TEST_F(DtorCorrectnessTest, VerifyFiringDtors) {
  // Create the listener and our firer:
  AutoRequired<MyCtorDtorListener> listener;
  AutoFired<CtorDtorListener> cdl;

  // Try firing some events and validate the invariant:
  cdl(&CtorDtorListener::DoFired)(CtorDtorCopyCounter());
  EXPECT_EQ(0, CtorDtorCopyCounter::s_count) << "Counter mismatch under event firing";

  cdl(&CtorDtorListener::DoDeferred)(CtorDtorCopyCounter());
  listener->Stop(true);
  EXPECT_EQ(0, CtorDtorCopyCounter::s_count) << "Counter mismatch under deferred events";
}