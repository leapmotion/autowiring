// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"

class AutoFilterCollapseRulesTest:
  public testing::Test
{
public:
  AutoFilterCollapseRulesTest(void) {
    // All decorator tests must run from an initiated context
    AutoCurrentContext()->Initiate();
  }
};

TEST_F(AutoFilterCollapseRulesTest, SharedPtrCollapse) {
  AutoRequired<AutoPacketFactory> factory;

  int contRefCallCount = 0;
  *factory += [&](const int& dataIn) {
    contRefCallCount++;
  };

  int sharedPtrRefCallCount = 0;
  std::shared_ptr<const int> dataInVal;
  *factory += [&](std::shared_ptr<const int> dataIn) {
    sharedPtrRefCallCount++;
    dataInVal = dataIn;
  };

  int constr_int = 222;
  std::shared_ptr<int> shared_int = std::make_shared<int>(232);
  ASSERT_TRUE(static_cast<bool>(shared_int));

  // Decorate(type X) calls AutoFilter(const type& X)
  // Decorate(type X) calls AutoFilter(shared_ptr<type> X)
  {
    auto packet = factory->NewPacket();
    packet->Decorate(constr_int);
    ASSERT_EQ(1, contRefCallCount) << "Called const reference method " << contRefCallCount << " times";
    ASSERT_EQ(1, sharedPtrRefCallCount) << "Called shared pointer method " << sharedPtrRefCallCount << " times";
  }
  contRefCallCount = 0;
  sharedPtrRefCallCount = 0;

  // Decorate(shared_ptr<type> X) calls AutoFilter(const type& X)
  // Decorate(shared_ptr<type> X) calls AutoFilter(shared_ptr<type> X)
  // NOTE: Decorate(shared_ptr<T> X) shares ownership of X instead of copying.
  {
    auto packet = factory->NewPacket();
    packet->Decorate(shared_int);
    ASSERT_EQ(1, contRefCallCount) << "Called const reference method " << contRefCallCount << " times";
    ASSERT_EQ(1, sharedPtrRefCallCount) << "Called shared pointer method " << sharedPtrRefCallCount << " times";
    ASSERT_FALSE(shared_int.unique()) << "Argument of Decorate should be shared, not copied, when possible";
  }
  contRefCallCount = 0;
  sharedPtrRefCallCount = 0;

  // DecorateImmediate(type X) calls AutoFilter(const type& X)
  // DecorateImmediate(type X) DOES NOT CALL AutoFilter(shared_ptr<type> X)
  // NOTE: This case is invalid, since DecorateImmediate assumes no validity of X after the function call,
  // so the construction of a shared_ptr from &X would violate the contract of shared_ptr type.
  // If an AutoFilter method assumed the validity of shared_ptr<type> Y, a copy could be made that might
  // become invalid.
  // NOTE: DecorateImmediate(shared_ptr<type> X) is unnecessary (a static_assert will be called).
  // Decorate(shared_ptr<type> X) will share the reference to X instead of making a copy.
  {
    auto packet = factory->NewPacket();
    packet->DecorateImmediate(constr_int);
    ASSERT_EQ(1, contRefCallCount) << "Called const reference method " << contRefCallCount << " times";
    ASSERT_EQ(1, sharedPtrRefCallCount) << "Shared pointer method not called as expected";
    ASSERT_EQ(nullptr, dataInVal) << "A non-null shared pointer was received by an AutoFilter method during a DecorateImmediate call";
  }
  contRefCallCount = 0;
  sharedPtrRefCallCount = 0;
}

TEST_F(AutoFilterCollapseRulesTest, ConstCollapse) {
  AutoRequired<AutoPacketFactory> factory;

  int callCount = 0;
  *factory += [&](const int& dataIn) {
    callCount++;
  };
  
  // Test const shared_ptr
  auto packet1 = factory->NewPacket();
  
  std::shared_ptr<const int> dec1 = std::make_shared<const int>(42);
  packet1->Decorate(dec1);
  
  ASSERT_EQ(1, callCount) << "'const T' decoration didn't resolve to 'T'";
  
  // Test const value
  auto packet2 = factory->NewPacket();
  
  const int dec2 = 42;
  packet2->Decorate(dec2);
  
  ASSERT_EQ(2, callCount) << "'const T' decoration didn't resolve to 'T'";
}

TEST_F(AutoFilterCollapseRulesTest, SharedPointerAliasingRules) {
  AutoRequired<AutoPacketFactory> factory;
  bool gen1Called = false;
  bool gen2Called = false;

  *factory += [&gen1Called](AutoPacket&, std::shared_ptr<const int>) {
    gen1Called = true;
  };
  *factory += [&gen2Called](AutoPacket&, int) {
    gen2Called = true;
  };

  auto packet = factory->NewPacket();
  packet->Decorate<int>(55);

  ASSERT_TRUE(gen1Called) << "AutoFilter accepting a shared pointer was not called as expected";
  ASSERT_TRUE(gen2Called) << "AutoFilter accepting a decorated type was not called as expected";
}

TEST_F(AutoFilterCollapseRulesTest, AutoFilterSharedAliasingRules) {
  AutoRequired<FilterGen<int>> consumes;
  AutoRequired<AutoPacketFactory> factory;

  *factory += [&](std::shared_ptr<int>& output) {
    output = std::make_shared<int>(55);
  };
  *factory += [&](int) {};

  // Decorate the packet, verify attribute presence:
  auto packet = factory->NewPacket();
  ASSERT_TRUE(packet->Has<int>()) << "Filter producing a shared pointer of type int did not correctly collapse to the basic int type";
  ASSERT_EQ(55, std::get<0>(consumes->m_args)) << "Filter consuming a shared pointer output was not called as expected";
}

TEST_F(AutoFilterCollapseRulesTest, UnsatisfiableDecoration) {
  AutoRequired<AutoPacketFactory> factory;

  bool bInvoked = false;
  *factory += [&](std::shared_ptr<const Decoration<0>> dec) {
    ASSERT_FALSE(dec) << "Decoration was attached to the packet when it should not have been";
    bInvoked = true;
  };
  *factory += [](const Decoration<0>&) {
    FAIL() << "A filter was invoked when it should have been unsatisfiable";
  };

  // Kick off what should be a launch of the shared pointer-accepting input
  auto packet = factory->NewPacket();
  packet->Decorate(std::shared_ptr<Decoration<0>>());

  // Ensure the correct decoration was invoked
  ASSERT_TRUE(bInvoked) << "AutoFilter was not invoked in an unsatisfiable case as expected";
}

TEST_F(AutoFilterCollapseRulesTest, SharedOutSelfAutoPrev) {
  AutoRequired<AutoPacketFactory> factory;
  AutoCurrentContext()->Initiate();

  auto hello = std::make_shared<std::string>("Hello world!");
  size_t nMsgs = 0;
  *factory += [&](auto_prev<std::string> lastMsg, std::shared_ptr<std::string>& msg) {
    msg = hello;
    if (lastMsg)
      nMsgs++;
  };

  auto packet1 = factory->NewPacket();
  auto packet2 = factory->NewPacket();

  ASSERT_TRUE(packet1->Has<std::string>());
  ASSERT_TRUE(packet2->Has<std::string>());
  ASSERT_EQ(1UL, nMsgs) << "auto_prev was not correctly set when a filter produces a shared_ptr output";
}

struct Base {};
struct Derived : public Base{};

TEST_F(AutoFilterCollapseRulesTest, AutoPrevInheritance) {
  AutoRequired<AutoPacketFactory> factory;
  AutoCurrentContext()->Initiate();
  
  size_t counter = 0;
  
  *factory += [&](const std::vector<Base>& base, auto_prev<std::vector<Derived>> prev, std::vector<Derived>& deriv) {
    deriv.emplace_back();
    
    if (counter) {
      ASSERT_FALSE(prev->empty()) << "auto_prev not called";
    }
    
    counter++;
  };
  
  for (size_t i = 0; i < 10; ++i) {
    auto packet = factory->NewPacket();
    std::vector<Base> baseVec{Base(), Base(), Base()};
    packet->Decorate(baseVec);
  }
}

TEST_F(AutoFilterCollapseRulesTest, CoreContextArg) {
  AutoCurrentContext ctxt;
  AutoRequired<AutoPacketFactory> factory;

  bool match1 = false;
  bool match2 = false;

  *factory += [ctxt, &match1](CoreContext& ctxtArg) {
    match1 = ctxt.get() == &ctxtArg;
  };
  *factory += [ctxt, &match2](std::shared_ptr<CoreContext> ctxtArg) {
    match2 = ctxt.get() == ctxtArg.get();
  };

  auto packet = factory->NewPacket();
  ASSERT_TRUE(match1) << "Reference input argument did not match expected";
  ASSERT_TRUE(match2) << "Shared pointer input argument did not match expectation";
}

static_assert(
  std::is_same<
    auto_arg<Decoration<0>>::id_type,
    auto_arg<std::shared_ptr<const Decoration<0>>>::id_type
  >::value,
  "ID type of a const shared_ptr did not match the fundamental type"
);

static_assert(
  std::is_same<
    auto_arg<Decoration<0>>::id_type,
    auto_arg<const std::shared_ptr<Decoration<0> const>&>::id_type
  >::value,
  "ID type of a const shared_ptr const reference did not match the fundamental type"
);

TEST_F(AutoFilterCollapseRulesTest, MultiConstCollapse) {
  AutoRequired<AutoPacketFactory> factory;

  auto called = std::make_shared<bool>(false);
  *factory += [called] (const std::shared_ptr<Decoration<0> const>&) {
    *called = true;
  };

  auto packet = factory->NewPacket();

  ASSERT_FALSE(*called) << "Const shared pointer const reference variant invoked prematurely";
  packet->Decorate(Decoration<0>{});
  ASSERT_TRUE(*called) << "Const shared pointer const reference variant not invoked";
}
