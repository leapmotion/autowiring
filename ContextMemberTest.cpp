#include "stdafx.h"
#include "ContextMemberTest.h"
#include "TestFixtures/SimpleObject.h"
#include "TestFixtures/SimpleThreaded.h"

class IsAContextMember:
  public ContextMember
{};

TEST_F(ContextMemberTest, AlienMembership) {
  // Create a new context, and construct an arbitrary ContextMember in it
  AutoCreateContext subCtxt;
  auto cc = subCtxt->Inject<IsAContextMember>();

  ASSERT_EQ(subCtxt, cc->GetContext()) << "ContextMember did not correctly obtain a pointer to its enclosing context";
}

class HasAFewSlots:
  public ContextMember
{
public:
  HasAFewSlots(void):
    ContextMember("HasAFewSlots")
  {}

  Autowired<SimpleThreadedT<int>> m_sthread1;
  Autowired<SimpleThreadedT<long>> m_sthread2;
  Autowired<SimpleObject> m_sobj;
};

TEST_F(ContextMemberTest, VerifyDetectedMembers)
{
  AutoRequired<HasAFewSlots> m_slots;
}
