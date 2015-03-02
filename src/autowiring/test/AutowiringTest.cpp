// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "gtest-all-guard.hpp"
#include "TestFixtures/SimpleObject.hpp"
#include "TestFixtures/SimpleReceiver.hpp"
#include <autowiring/Autowired.h>
#include <autowiring/CoreThread.h>

int main(int argc, const char* argv []) {
  return autotesting_main(argc, argv);
}

class AutowiringTest:
  public testing::Test
{};

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

class PublicBase {
public:
  PublicBase(void) {}
  virtual ~PublicBase(void) {}
};

class PrivateBase {
public:
  PrivateBase(void) {}
  virtual ~PrivateBase(void) {}
};

class Derived : public PublicBase, private PrivateBase {
public:
  Derived(void) {}
  ~Derived(void) {}
};

TEST_F(AutowiringTest, TestFailureOfDynamicCast) {
  Derived d;
  PublicBase *pub = static_cast<PublicBase*>(&d);
  ASSERT_EQ(dynamic_cast<PrivateBase*>(pub), nullptr) << "Dynamic cast failed to give nullptr when cross casting to a private base class";
  static_assert(!std::is_base_of<Derived, PrivateBase>::value, "is_base_of said a private base was a base");
}

class CompletelyUndefinedType;

TEST_F(AutowiringTest, CanAutowireCompletelyUndefinedType) {
  Autowired<CompletelyUndefinedType> cut;
  ASSERT_EQ(nullptr, cut.get_unsafe()) << "Autowiring of a completely undefined type succeeded unexpectedly";
}

class StaticNewInt;

StaticNewInt* CreateStaticNewIntImpl();
StaticNewInt* CreateStaticNewIntImpl(std::unique_ptr<int>);

class StaticNewInt:
  public CoreObject
{
protected:
  StaticNewInt(void){}
public:
  static StaticNewInt* New(std::unique_ptr<int> value) {
    return CreateStaticNewIntImpl(std::move(value));
  }

  static StaticNewInt* New(void) {
    return CreateStaticNewIntImpl();
  }

  virtual int getValue(void) = 0;
};

class StaticNewIntImpl:
  public StaticNewInt
{
public:
  StaticNewIntImpl(void):
    m_value(new int(42))
  {}
  StaticNewIntImpl(std::unique_ptr<int> val):
    m_value(std::move(val))
  {}

  int getValue(void) override {
    return *m_value;
  }

  std::unique_ptr<int> m_value;
};

StaticNewInt* CreateStaticNewIntImpl(){
  return new StaticNewIntImpl();
}
StaticNewInt* CreateStaticNewIntImpl(std::unique_ptr<int> val){
  return new StaticNewIntImpl(std::move(val));
}

TEST_F(AutowiringTest, StaticNewWithArgs) {
  const bool static_new = has_static_new<StaticNewInt, typename std::unique_ptr<int>>::value;
  ASSERT_TRUE(static_new) << "has_static_new didn't correctly identify static New()";

  {
    AutoCreateContext ctxt;
    ASSERT_NO_THROW(ctxt->Inject<StaticNewInt>()) << "Exception throws while injecting member";
    AutowiredFast<StaticNewInt> obj(ctxt);
    EXPECT_EQ(42, obj->getValue()) << "Wrong constructor called";
    ctxt->SignalShutdown(true);
  }
  {
    AutoCreateContext ctxt;
    ASSERT_NO_THROW(auto obj = ctxt->Inject<StaticNewInt>(std::unique_ptr<int>(new int(1337)))) << "Exception throws while injecting member";
    AutowiredFast<StaticNewInt> obj(ctxt);
    EXPECT_EQ(1337, obj->getValue()) << "Wrong constructor called";
    ctxt->SignalShutdown(true);
  }
}

TEST_F(AutowiringTest, NullDereferenceAttempt) {
  Autowired<SimpleObject> co;
  ASSERT_ANY_THROW(*co) << "A dereference attempt on a CoreObject did not throw an exception as expected";
  ASSERT_ANY_THROW(co->one) << "A dereference attempt on a CoreObject did not throw an exception as expected";
}

TEST_F(AutowiringTest, FastNullDereferenceAttempt) {
  AutowiredFast<SimpleObject> co;
  ASSERT_ANY_THROW(*co) << "A dereference attempt on a CoreObject did not throw an exception as expected";
  ASSERT_ANY_THROW(co->one) << "A dereference attempt on a CoreObject did not throw an exception as expected";
}