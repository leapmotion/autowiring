// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/MultiInherit.hpp"
#include <autowiring/autowiring.h>
#include <autowiring/CoreContext.h>

class MultiInheritTest:
  public testing::Test
{
};

class Shared {
public:
  int m_i = 100;
};

class Base {
public:
  AutoRequired<Shared> m_member;
};

class MultiInheritDerived:
  public Base
{
public:
  MultiInheritDerived(void) {
    EXPECT_TRUE(Base::m_member.IsAutowired()) << "Base AutoRequired member was not initialized properly";
    EXPECT_EQ(100, Base::m_member->m_i) << "Autowired instance was not properly constructed";
    EXPECT_TRUE(m_secondMember.IsAutowired()) << "Failed to autowire a type which should have been injected in this context";
    EXPECT_EQ(m_member.get(), m_secondMember.get()) << "Autowiring idempotency was violated";
  }

  Autowired<Shared> m_secondMember;
};

TEST_F(MultiInheritTest, VerifyCast) {
  // Create a dummy context and make it current:
  AutoCreateContext ctxt;
  CurrentContextPusher pshr(ctxt);

  // Insert a MultiInherit object:
  auto obj = ctxt->Inject<MultiInherit>();

  // Autowire in the pObj:
  Autowired<MultiInherit> wiredPobj;
  ASSERT_TRUE(wiredPobj.IsAutowired()) << "Autowiring failed for a multi-inheritance object";

  // Verify that we get a pObj back with correct casting:
  ASSERT_EQ(obj.get(), wiredPobj.get()) << "Autowiring failed on a multiple inheritance object";
}

TEST_F(MultiInheritTest, VerifyBaseInitializer) {
  AutoCreateContext ctxt;
  CurrentContextPusher pshr(ctxt);

  MultiInheritDerived derived;

  // Expect that something autowires when we're done at least:
  ASSERT_TRUE(derived.m_member.IsAutowired());
}
