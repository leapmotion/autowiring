// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
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

class AcceptsConstReference {
public:
  AcceptsConstReference(void) :
    m_called(0)
  {}

  int m_called;

  void AutoFilter(const int& dataIn) {
    ++m_called;
  }
};

class AcceptsSharedPointer {
public:
  AcceptsSharedPointer(void) :
    m_called(0)
  {}

  int m_called;

  void AutoFilter(std::shared_ptr<const int> dataIn) {
    ++m_called;
  }
};

TEST_F(AutoFilterCollapseRulesTest, SharedPtrCollapse) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<AcceptsConstReference> constr_filter;
  AutoRequired<AcceptsSharedPointer> shared_filter;

  int constr_int = 222;
  std::shared_ptr<int> shared_int = std::make_shared<int>(232);
  ASSERT_TRUE(static_cast<bool>(shared_int));

  // Decorate(type X) calls AutoFilter(const type& X)
  // Decorate(type X) calls AutoFilter(shared_ptr<type> X)
  {
    auto packet = factory->NewPacket();
    packet->Decorate(constr_int);
    ASSERT_EQ(1, constr_filter->m_called) << "Called const reference method " << constr_filter->m_called << " times";
    ASSERT_EQ(1, shared_filter->m_called) << "Called shared pointer method " << shared_filter->m_called << " times";
  }
  constr_filter->m_called = 0;
  shared_filter->m_called = 0;

  // Decorate(shared_ptr<type> X) calls AutoFilter(const type& X)
  // Decorate(shared_ptr<type> X) calls AutoFilter(shared_ptr<type> X)
  // NOTE: Decorate(shared_ptr<T> X) shares ownership of X instead of copying.
  {
    auto packet = factory->NewPacket();
    packet->Decorate(shared_int);
    ASSERT_EQ(1, constr_filter->m_called) << "Called const reference method " << constr_filter->m_called << " times";
    ASSERT_EQ(1, shared_filter->m_called) << "Called shared pointer method " << shared_filter->m_called << " times";
    ASSERT_FALSE(shared_int.unique()) << "Argument of Decorate should be shared, not copied, when possible";
  }
  constr_filter->m_called = 0;
  shared_filter->m_called = 0;

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
    ASSERT_EQ(1, constr_filter->m_called) << "Called const reference method " << constr_filter->m_called << " times";
    ASSERT_EQ(0, shared_filter->m_called) << "Called shared pointer method " << shared_filter->m_called << " times";
  }
  constr_filter->m_called = 0;
  shared_filter->m_called = 0;
}

class UnnamedExternalClass;

class AcceptsUnnamedExternalClass {
public:
  void AutoFilter(const UnnamedExternalClass&) {}
};

class AcceptsUnnamedExternalClassSharedPtr {
public:
  void AutoFilter(std::shared_ptr<const UnnamedExternalClass>) {}
};

TEST_F(AutoFilterCollapseRulesTest, CanAcceptUndefinedSharedPointerInput) {
  AutoRequired<AcceptsUnnamedExternalClass> auec;
  AutoRequired<AcceptsUnnamedExternalClassSharedPtr> auecsp;
}

TEST_F(AutoFilterCollapseRulesTest, ConstCollapse) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<AcceptsConstReference> filter;
  
  // Test const shared_ptr
  auto packet1 = factory->NewPacket();
  
  std::shared_ptr<const int> dec1 = std::make_shared<const int>(42);
  packet1->Decorate(dec1);
  
  ASSERT_EQ(1, filter->m_called) << "'const T' decoration didn't resolve to 'T'";
  
  // Test const value
  auto packet2 = factory->NewPacket();
  
  const int dec2 = 42;
  packet2->Decorate(dec2);
  
  ASSERT_EQ(2, filter->m_called) << "'const T' decoration didn't resolve to 'T'";
}

TEST_F(AutoFilterCollapseRulesTest, SharedPointerAliasingRules) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<FilterGen<std::shared_ptr<const int>>> genFilter1;
  AutoRequired<FilterGen<int>> genFilter2;

  auto packet = factory->NewPacket();
  packet->Decorate<int>(55);

  ASSERT_EQ(1UL, genFilter1->m_called) << "AutoFilter accepting a shared pointer was not called as expected";
  ASSERT_EQ(1UL, genFilter2->m_called) << "AutoFilter accepting a decorated type was not called as expected";
}

class ProducesSharedPointer {
public:
  ProducesSharedPointer(void) :
    m_called(0)
  {}

  int m_called;

  void AutoFilter(std::shared_ptr<int>& output) {
    ++m_called;
    output = std::make_shared<int>(55);
  }
};

TEST_F(AutoFilterCollapseRulesTest, AutoFilterSharedAliasingRules) {
  AutoRequired<ProducesSharedPointer> produces;
  AutoRequired<FilterGen<int>> consumes;
  AutoRequired<AutoPacketFactory> factory;

  // Decorate the packet, verify attribute presence:
  auto packet = factory->NewPacket();
  ASSERT_TRUE(packet->Has<int>()) << "Filter producing a shared pointer of type int did not correctly collapse to the basic int type";
  ASSERT_EQ(55, std::get<0>(consumes->m_args)) << "Filter consuming a shared pointer output was not called as expected";
}
