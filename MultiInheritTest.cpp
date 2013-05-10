#include "stdafx.h"
#include "MultiInheritTest.h"
#include "Autowired.h"
#include "CoreContext.h"
#include "TestFixtures/MultiInherit.h"

class Shared {
public:
  Shared(void) {
    m_i = 100;
  }

  int m_i;
};

class Base {
public:
  AutoRequired<Shared> m_member;
};

class Derived:
  public Base
{
public:
  Derived(void) {
    EXPECT_TRUE(Base::m_member.IsAutowired()) << "Base AutoRequired member was not initialized properly";
    EXPECT_EQ(100, Base::m_member->m_i) << "Autowired instance was not properly constructed";
    EXPECT_EQ(m_member, m_secondMember) << "Autowiring idempotency was violated";
  }

  Autowired<Shared> m_secondMember;
};

TEST_F(MultiInheritTest, VerifyCast) {
  // Create a dummy context and make it current:
  Autowired<CoreContext> ctxt;
  ctxt.Create();
  CurrentContextPusher pshr(ctxt);

  // Make sure that we still have a context:
  ASSERT_TRUE(ctxt.IsAutowired()) << "Create call must create a context";

  // Insert a MultiInherit object:
  std::shared_ptr<MultiInherit> obj(new MultiInherit());
  ctxt->Add(obj);

  // Autowire in the pObj:
  Autowired<MultiInherit> wiredPobj;
  ASSERT_TRUE(wiredPobj.IsAutowired()) << "Autowiring failed for a multi-inheritance object";

  // Verify that we get a pObj back with correct casting:
  EXPECT_EQ(obj, wiredPobj) << "Autowiring failed on a multiple inheritance object";
}

TEST_F(MultiInheritTest, VerifyBaseInitializer) {
  Autowired<CoreContext> ctxt;
  ctxt.Create();
  CurrentContextPusher pshr(ctxt);

  Derived derived;

  // Expect that something autowires when we're done at least:
  EXPECT_TRUE(derived.m_member.IsAutowired());
}
