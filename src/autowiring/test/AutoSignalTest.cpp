// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/autowiring.h>

class AutoSignalTest:
  public testing::Test
{};

class RaisesASignal {
public:
  autowiring::signal<void(int)> signal;
};

TEST_F(AutoSignalTest, SimpleSignalTest) {
  autowiring::signal<void(int)> signal;

  bool handler_called = false;
  int val = 0;

  // Register a handler directly on the signal:
  auto* registration =
    signal += [&](int v) {
      handler_called = true;
      val = v;
    };

  // Trivially raise the signal:
  signal(101);
  ASSERT_TRUE(handler_called) << "Handler was not called on a stack-allocated signal";
  ASSERT_EQ(101, val) << "Handler did not correctly copy a passed value";

  // Unregister and verify we can still raise the signal:
  signal -= registration;

  handler_called = false;
  signal(102);
  ASSERT_FALSE(handler_called) << "Handler was called after being unregistered";
}

TEST_F(AutoSignalTest, SignalWithAutowiring) {
  bool handler_called = false;
  int val = 202;

  {
    Autowired<RaisesASignal> ras;

    // Register a signal handler:
    ras(&RaisesASignal::signal) += [&](int v) {
      handler_called = true;
      val = v;
    };

    // Inject type type after the signal has been registered
    AutoRequired<RaisesASignal>();

    // Now raise the signal, see what happens:
    ras->signal(55);

    // Verify that the handler got called with the correct value:
    ASSERT_TRUE(handler_called) << "Signal handler was not invoked";
    ASSERT_EQ(55, val) << "Signal handler not called with the correct parameter as expected";
  }

  // Raise the signal again, this should not cause anything to break:
  Autowired<RaisesASignal> ras;
  handler_called = false;
  ras->signal(99);
  ASSERT_FALSE(handler_called) << "A handler was unexpectedly called after it should have been destroyed";
}

TEST_F(AutoSignalTest, MultipleSlotsTest) {
  autowiring::signal<void(void)> signal;
  
  bool handler_called1 = false;
  bool handler_called2 = false;
  
  auto* registration1 =
  signal += [&] {
    handler_called1 = true;
  };
  
  // Registration 2
  signal += [&] {
    handler_called2 = true;
  };
  
  // Trivially raise the signal:
  signal();
  ASSERT_TRUE(handler_called1) << "Handler 1 was not called on a stack-allocated signal";
  ASSERT_TRUE(handler_called2) << "Handler 2 was not called on a stack-allocated signal";
  
  // Unregister the first signal and reset the variables
  signal -= registration1;
  
  handler_called1 = false;
  handler_called2 = false;
  
  // Verify that registration 2 can still receive the signals
  signal();
  ASSERT_FALSE(handler_called1) << "Handler 1 was called after being unregistered";
  ASSERT_TRUE(handler_called2) << "Handler 2 was inadvertently unregistered";
}

TEST_F(AutoSignalTest, RaiseASignalWithinASlotTest) {
  autowiring::signal<void(void)> signal1;
  autowiring::signal<void(void)> signal2;
  
  bool handler_called1 = false;
  bool handler_called2 = false;
  
  signal1 += [&] {
    handler_called1 = true;
    signal2();
  };
  
  signal2 += [&] {
    handler_called2 = true;
  };
  
  // Trivially raise the signal:
  signal1();
  ASSERT_TRUE(handler_called1) << "Handler 1 was not called on a stack-allocated signal";
  ASSERT_TRUE(handler_called2) << "Handler 2 was not called on a stack-allocated signal";
}

TEST_F(AutoSignalTest, NodeRemoval) {
  autowiring::signal<void(void)> signal1;
  autowiring::signal<void(void)> signal2;

  bool handler_called1 = false;
  bool handler_called2 = false;

  auto* registration1 = signal1 += [&] { handler_called1 = true; };
  auto* registration2 = signal2 += [&] { handler_called2 = true; };
  
  ASSERT_ANY_THROW(signal1 -= registration2) << "Removing a registration from a different signal than it was registered to failed to throw an exception";

  registration1->remove();
  signal1();
  signal2();
  
  ASSERT_FALSE(handler_called1) << "Handler1 was called after being unregistered";
  ASSERT_TRUE(handler_called2) << "Handler2 was removed after an invalid -= operation";
}

TEST_F(AutoSignalTest, CallOrdering) {
  autowiring::signal<void(void)> signal1;

  bool handler_called1 = false;
  bool handler_called2 = false;

  //handlers are inserted at the begining, so this will be called last.
  auto* registration1 = signal1 += [&] {
    ASSERT_TRUE(handler_called2);
    handler_called1 = true;
  };
  auto* registration2 = signal1 += [&] { handler_called2 = true; };

  signal1();

  ASSERT_TRUE(handler_called1) << "Handler1 was called after being unregistered";
  ASSERT_TRUE(handler_called2) << "Handler2 was removed after an invalid -= operation";
}

TEST_F(AutoSignalTest, CallInsertion) {
  autowiring::signal<void(void)> signal1;

  bool handler_called1 = false;
  bool handler_called2 = false;

  auto* registration1 = signal1 += [&] { handler_called1 = true; };

  //when += to a registration object, the new one is appended.
  auto* registration2 = *registration1 += [&] {
    ASSERT_TRUE(handler_called1);
    handler_called2 = true; };

  signal1();

  ASSERT_TRUE(handler_called1) << "Handler1 was called after being unregistered";
  ASSERT_TRUE(handler_called2) << "Handler2 was removed after an invalid -= operation";
}