// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutowiringTest.hpp"
#include "gtest-all-guard.h"
#include "TestFixtures/SimpleObject.hpp"
#include "TestFixtures/SimpleReceiver.hpp"
#include <autowiring/Autowired.h>
#include <autowiring/CoreThread.h>

int main(int argc, const char* argv []) {
  return autotesting_main(argc, argv);
}

TEST_F(AutowiringTest, VerifyAutowiredFast) {
  // Add an object:
  AutoCurrentContext()->Inject<SimpleObject>();

  // Verify that AutowiredFast can find this object
  AutowiredFast<SimpleObject> sobj;
  ASSERT_TRUE(sobj.IsAutowired()) << "Failed to autowire an object which was just injected into a context";
}

TEST_F(AutowiringTest, VerifyAutowiredFastNontrivial) {
  // This will cause a cache entry to be inserted into the CoreContext's memoization system.
  // If there is any improper or incorrect invalidation in that system, then the null entry
  // will create problems when we attempt to perform an AutowiredFast later on.
  AutowiredFast<CallableInterface>();

  // Now we add the object
  AutoRequired<SimpleReceiver>();

  // Verify that AutowiredFast can find this object from its interface
  AutowiredFast<CallableInterface> ci;
  ASSERT_TRUE(ci.IsAutowired()) << "Failed to autowire an interface advertised by a newly-inserted object";
}

template<int N>
class AutowiresOneThingThenQuits:
  public CoreThread
{
public:
  AutoRequired<std::mutex> m_ctxtLock;

  void Run(void) override {
    std::lock_guard<std::mutex> lk(*m_ctxtLock);
    Autowired<SimpleObject>();
  }
};

void InjectAll(void) {
  AutoRequired<AutowiresOneThingThenQuits<0>>();
  AutoRequired<AutowiresOneThingThenQuits<1>>();
  AutoRequired<AutowiresOneThingThenQuits<2>>();
  AutoRequired<AutowiresOneThingThenQuits<3>>();
  AutoRequired<AutowiresOneThingThenQuits<4>>();
  AutoRequired<AutowiresOneThingThenQuits<5>>();
  AutoRequired<AutowiresOneThingThenQuits<6>>();
  AutoRequired<AutowiresOneThingThenQuits<7>>();
  AutoRequired<AutowiresOneThingThenQuits<8>>();
  AutoRequired<AutowiresOneThingThenQuits<9>>();
  AutoRequired<AutowiresOneThingThenQuits<10>>();
  AutoRequired<AutowiresOneThingThenQuits<11>>();
  AutoRequired<AutowiresOneThingThenQuits<12>>();
  AutoRequired<AutowiresOneThingThenQuits<13>>();
  AutoRequired<AutowiresOneThingThenQuits<14>>();
  AutoRequired<AutowiresOneThingThenQuits<15>>();
}

TEST_F(AutowiringTest, PathologicalAutowiringRace) {
  AutoRequired<std::mutex> lock;

  (std::lock_guard<std::mutex>)*lock,
  InjectAll();

  // Now insert at about the same time as other threads are waking up.  If there are synchronization problems
  // in spin-up or tear-down, 
  AutoRequired<SimpleObject>();
}