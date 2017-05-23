// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/SimpleObject.hpp"
#include "TestFixtures/SimpleReceiver.hpp"
#include <autotesting/gtest-all-guard.hpp>
#include <autowiring/autowiring.h>
#include <autowiring/AutowiringDebug.h>
#include <autowiring/CoreThread.h>

// Version number should have at least two dots, three digits, and a null terminator
static_assert(sizeof(AUTOWIRING_VERSION_STR) >= 2 + 3 + 1, "Version number string appears to be incorrect");

// Version number field itself must be at least 0x10003, which corresponds to v1.0.3
static_assert(0x10003 <= AUTOWIRING_VERSION, "Version number appears to have gone backwards");

// Other version number fields should be at least one character each
static_assert(sizeof(AUTOWIRING_VERSION_MAJOR) >= 2, "Major version number component appears to be incorrect");
static_assert(sizeof(AUTOWIRING_VERSION_MINOR) >= 2, "Minor version number component appears to be incorrect");
static_assert(sizeof(AUTOWIRING_VERSION_PATCH) >= 2, "Patch version number component appears to be incorrect");

int main(int argc, const char* argv []) {
  autowiring::dbg::DebugInit();
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
  ASSERT_NE(sobj, nullptr);
}

TEST_F(AutowiringTest, VerifyAutowiredFastNontrivial) {
  // This will cause a cache entry to be inserted into the CoreContext's memoization system.
  // If there is any improper or incorrect invalidation in that system, then the null entry
  // will create problems when we attempt to perform an AutowiredFast later on.
  AutowiredFast<CallableInterface> ciEmpty;
  ASSERT_FALSE(ciEmpty.IsAutowired()) << "An entry was autowired prematurely";

  // Now we add the object
  AutoRequired<SimpleReceiver>();
  ASSERT_FALSE(ciEmpty.IsAutowired()) << "An AutowiredFast field was incorrectly satisfied post-hoc";
  ASSERT_EQ(nullptr, ciEmpty);

  // Verify that AutowiredFast can find this object from its interface
  AutowiredFast<CallableInterface> ci;
  ASSERT_TRUE(ci.IsAutowired()) << "Failed to autowire an interface advertised by a newly-inserted object";
  ASSERT_NE(nullptr, ci);
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

class DynamicCastDerivedType : public PublicBase, private PrivateBase {
public:
  DynamicCastDerivedType(void) {}
  ~DynamicCastDerivedType(void) {}

  FRIEND_TEST(AutowiringTest, PostHocTypeAdvertisement);
};

TEST_F(AutowiringTest, TestFailureOfDynamicCast) {
  DynamicCastDerivedType d;
  PublicBase* pub = static_cast<PublicBase*>(&d);
  PrivateBase* priv = dynamic_cast<PrivateBase*>(pub);
  ASSERT_EQ(nullptr, priv) << "Dynamic cast failed to give nullptr when cross casting to a private base class";
  static_assert(!std::is_base_of<DynamicCastDerivedType, PrivateBase>::value, "is_base_of said a private base was a base");
}

TEST_F(AutowiringTest, PostHocTypeAdvertisement) {
  AutoCurrentContext ctxt;
  AutoRequired<DynamicCastDerivedType> dcdt;
  Autowired<PrivateBase> pb;

  ASSERT_FALSE(pb.IsAutowired()) << "Private base type was present in the context before it was advertised";

  ctxt->Add(
    std::shared_ptr<PrivateBase>{
      dcdt,
      static_cast<PrivateBase*>(dcdt.get())
    }
  );
  ASSERT_TRUE(pb.IsAutowired()) << "Type was not autowired after an injection event took place that should have made it available";
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

static_assert(
  autowiring::has_static_new<StaticNewInt>::value,
  "Default static new not detected on StaticNewInt"
);

static_assert(
  autowiring::has_static_new<StaticNewInt, std::unique_ptr<int>>::value,
  "Static new with arguments not detected on StaticNewInt"
);

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
  const bool static_new = autowiring::has_static_new<StaticNewInt, typename std::unique_ptr<int>>::value;
  ASSERT_TRUE(static_new) << "has_static_new didn't correctly identify static New()";

  {
    AutoCreateContext ctxt;
    ASSERT_NO_THROW(ctxt->Inject<StaticNewInt>()) << "Exception throws while injecting member";
    AutowiredFast<StaticNewInt> obj(ctxt);
    ASSERT_EQ(42, obj->getValue()) << "Wrong constructor called";
    ctxt->SignalShutdown(true);
  }
  {
    AutoCreateContext ctxt;
    ASSERT_NO_THROW(auto obj = ctxt->Inject<StaticNewInt>(std::unique_ptr<int>(new int(1337)))) << "Exception throws while injecting member";
    AutowiredFast<StaticNewInt> obj(ctxt);
    ASSERT_EQ(1337, obj->getValue()) << "Wrong constructor called";
    ctxt->SignalShutdown(true);
  }
}

struct PureVirtualStruct {
  virtual void PureVirtualFunction() = 0;
};

struct VSImpl : public PureVirtualStruct {
  void PureVirtualFunction() override {
    (void)0;
  }
};

struct ConcreteStruct {
  ConcreteStruct(PureVirtualStruct& pRef): m_ref(pRef) {}
  PureVirtualStruct& m_ref;
};

static_assert(!autowiring::has_static_new<ConcreteStruct, PureVirtualStruct&>::value, "Found static new on type that doesn't have one");

TEST_F(AutowiringTest, AbstractArgTest) {
  const bool static_new = autowiring::has_static_new<ConcreteStruct, PureVirtualStruct&>::value;
  ASSERT_FALSE(static_new);
  {
    AutoCreateContext ctxt;
    ctxt->Inject<VSImpl>();
    AutowiredFast<PureVirtualStruct> pvs(ctxt);
    ctxt->Inject<ConcreteStruct>(*pvs);
  }
}

namespace {
  class InjectsItself:
    public CoreObject
  {
  public:
    InjectsItself(bool recurse) {
      if(recurse)
        AutoConstruct<InjectsItself>{false};
    }
  };
}

TEST_F(AutowiringTest, AutowiresItself) {
  // Try to overtake:
  ASSERT_NO_THROW(AutoConstruct<InjectsItself> bii(true)) << "An overtaken constructor incorrectly caused an exception to be thrown";
}

TEST_F(AutowiringTest, NullDereferenceTest) {
  Autowired<SimpleObject> nwa;
  ASSERT_THROW(*nwa, autowiring::deref_error);
  try {
    (void)nwa->one;
    FAIL() << "Dereference error not thrown for a null Autowired field";
  }
  catch (autowiring::deref_error& dre) {
    ASSERT_STREQ("Attempted to dereference a null Autowired<SimpleObject>", dre.what());
  }
}

TEST_F(AutowiringTest, FastNullDereferenceTest) {
  AutowiredFast<SimpleObject> nwaFast;
  ASSERT_THROW(*nwaFast, autowiring::deref_error);
  try {
    (void)nwaFast->one;
    FAIL() << "Dereference error not thrown for a null AutowiredFast field";
  }
  catch (autowiring::deref_error& dre) {
    ASSERT_STREQ("Attempted to dereference a null AutowiredFast<SimpleObject>", dre.what());
  }
}

TEST_F(AutowiringTest, ComparisonOverloadTest) {
  Autowired<SimpleObject> sobj;

  // These two tests actually resolve to different overloads, so we need to ensure both expressions do get called
  ASSERT_EQ(nullptr, sobj);
  ASSERT_TRUE(sobj == nullptr);

  // The remainder of the test will be on the positive comparison case, so we can inject now
  AutoRequired<SimpleObject>{};

  // Same here as above
  ASSERT_NE(sobj, nullptr);
  ASSERT_NE(nullptr, sobj);

  // Another test to verify our shared pointer casts all resolve properly
  ASSERT_EQ(static_cast<const std::shared_ptr<SimpleObject>&>(sobj), sobj);
  ASSERT_EQ(sobj, static_cast<const std::shared_ptr<SimpleObject>&>(sobj));

  // We should also be able to handle casted versions of the same
  std::shared_ptr<ContextMember> base;
  ASSERT_NE(base, sobj);
  ASSERT_NE(sobj, base);
  base = sobj;
  ASSERT_EQ(base, sobj);
  ASSERT_EQ(sobj, base);

  // Finally we also support primitive pointer comparisons
  ASSERT_EQ(sobj.get(), sobj);
  ASSERT_EQ(sobj, sobj.get());
}
