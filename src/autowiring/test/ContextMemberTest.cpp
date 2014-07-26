// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ContextMemberTest.hpp"
#include "TestFixtures/SimpleObject.hpp"
#include "TestFixtures/SimpleThreaded.hpp"

class IsAContextMember:
  public ContextMember
{};

static_assert(std::is_base_of<ContextMember, IsAContextMember>::value, "Expected test class to be a ContextMember");

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

#define offsetof_nowarn(s,m)   (size_t)&reinterpret_cast<const volatile char&>((((s *)0)->m))

TEST_F(ContextMemberTest, VerifyDetectedMembers)
{
  AutoRequired<HasAFewSlots> hasAFew;

  // Slots defined in reverse order here, because that's how they will be present in the collection
  SlotInformation expected [] = {
    SlotInformation(nullptr, typeid(SimpleObject), offsetof_nowarn(HasAFewSlots, m_sobj), false),
    SlotInformation(nullptr, typeid(SimpleThreadedT<long>), offsetof_nowarn(HasAFewSlots, m_sthread2), false),
    SlotInformation(nullptr, typeid(SimpleThreadedT<int>), offsetof_nowarn(HasAFewSlots, m_sthread1), false)
  };

  // Validate all pointers are what we expect to find, and in the right order
  size_t ct = 0;

  for(auto cur = SlotInformationStump<HasAFewSlots>::s_stump.pHead; cur; cur = cur->pFlink) {
    // Validate slot types:
    ASSERT_EQ(expected[ct].type, cur->type) << "Slot number " << ct << " had a mismatched type";
    ASSERT_EQ(expected[ct].slotOffset, cur->slotOffset) << "Slot number " << ct << " had a mismatched offset";
    ct++;
  }
  ASSERT_EQ(3UL, ct) << "Slot registration detected an insufficient number of created slots";
}

class AutoRequiresSomething
{
  AutoRequired<HasAFewSlots> m_slots;
  Autowired<SimpleObject> m_simpleObj;
};

TEST_F(ContextMemberTest, RecursiveRelationshipNoContextMember) {
  // Create our root type:
  AutoRequired<AutoRequiresSomething> ars;
  AutoRequired<HasAFewSlots> hasAFew;

  // Now verify that we only get the type we expect, and none of the transitive types:
  size_t ct = 0;
  for(auto cur = SlotInformationStump<AutoRequiresSomething>::s_stump.pHead; cur; cur = cur->pFlink)
    ct++;
  ASSERT_LE(1UL, ct) << "Recursive construction of an AutoRequired type incorrectly mapped in slots in a created field";
  ASSERT_GE(ct, 1UL) << "Recursive construction failed to map a type after an AutoRequired field was constructed";

  // Descendant type should also be correctly initialized:
  ct = 0;
  for(auto cur = SlotInformationStump<HasAFewSlots>::s_stump.pHead; cur; cur = cur->pFlink)
    ct++;
  ASSERT_EQ(3UL, ct) << "Recursively AutoRequired type did not enumerate all of the expected slots upon reflection";
}

class AutowiresInCtor:
  public ContextMember
{
public:
  AutowiresInCtor(void) {
    Autowired<SimpleObject>();
  }

private:
  Autowired<SimpleObject> m_member;
};

TEST_F(ContextMemberTest, TransientAutowiring) {
  AutoRequired<AutowiresInCtor> aictor;

  size_t ct = 0;
  for(auto cur = SlotInformationStump<AutowiresInCtor>::s_stump.pHead; cur; cur = cur->pFlink)
    ct++;

  ASSERT_EQ(1UL, ct) << "An autowirable slot declared on the stack was incorrectly detected as being a type member";
}