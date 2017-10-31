// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/SimpleObject.hpp"
#include "TestFixtures/SimpleThreaded.hpp"
#include <autowiring/autowiring.h>
#include <autowiring/CoreContext.h>
#include <autowiring/once.h>
#include THREAD_HEADER

TEST(OnceTest, NoRepeatedCall) {
  autowiring::once_signal<void> o;

  int val = 0;
  o += [&] { val++; };

  o = true;
  ASSERT_EQ(1, val) << "Lambda not executed when flag was set";
  o = true;
  ASSERT_EQ(1, val) << "Lambda was executed too many times, once flag did not properly implement idempotence";
}

TEST(OnceTest, CallAfterSet) {
  autowiring::once_signal<void> o;

  int val1 = 0;
  o += [&] { val1++; };

  o = true;
  ASSERT_EQ(1, val1) << "Lambda not executed when a flag was set";

  int val2 = 0;
  o += [&] { val2++; };
  ASSERT_EQ(1, val2) << "Lambda not executed on assignment as expected";
}

TEST(OnceTest, NoLeaks_Asserted) {
  autowiring::once_signal<void> o;
  auto v = std::make_shared<bool>(false);
  o += [v] {};
  ASSERT_EQ(2UL, v.use_count()) << "Shared pointer not properly captured by once flag";

  o = true;
  ASSERT_TRUE(v.unique()) << "Shared pointer leaked by once flag after execution";
  o += [v] {};
  ASSERT_TRUE(v.unique()) << "Shared pointer leaked by once flag after post-set attachment";
}

TEST(OnceTest, NoLeaks_NotAsserted) {
  auto v = std::make_shared<bool>(false);
  {
    autowiring::once_signal<void()> x;
    x += [v] {};
  }
  ASSERT_TRUE(v.unique()) << "Signal did not destroy all attached lambdas on its destruction";
}

TEST(OnceTest, MultiLambdaPending) {
  autowiring::once_signal<void> o;
  std::atomic<int> x{ 0 };
  std::atomic<int> y{ 0 };
  bool go = true;

  std::thread v([&] {
    while(go) {
      o += [&] { x++; };
      y++;
    }
  });

  // Let the other thread zip around for awhile in both modes:
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  o = true;
  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  // Finally we verify that everything got hit as we expected:
  go = false;
  v.join();
  ASSERT_EQ(y, x) << "Not all pended lambdas were executed as expected";
}

class PrivateSignal {
public:
  autowiring::once_signal<PrivateSignal> sig;

  void signal(void) {
    sig.signal();
  }
};

TEST(OnceTest, SignalSetWhileCalling) {
  autowiring::once_signal<void> sig;
  sig += [&] {
    ASSERT_TRUE(sig) << "Flag was not set while handler was being invoked";
  };
  sig = true;
}

TEST(OnceTest, OwnedSignal) {
  PrivateSignal priv;

  bool hit = false;
  priv.sig += [&hit] { hit = true; };
  priv.signal();
  ASSERT_TRUE(hit) << "Private signal did not get set as expected";
}

TEST(OnceTest, UnregisterHandler) {
  autowiring::once_signal<void> sig;

  bool hit = false;
  auto reg = sig += [&hit] {
    hit = true;
  };
  sig -= reg;
  sig();
  ASSERT_FALSE(hit) << "Handler not unregistered as expected";
}
