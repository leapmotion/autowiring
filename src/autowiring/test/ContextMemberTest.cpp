// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/SimpleObject.hpp"
#include "TestFixtures/SimpleThreaded.hpp"
#include <thread>

class ContextMemberTest:
  public testing::Test
{};

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

#define offsetof_nowarn(s,m)   ((size_t)&reinterpret_cast<const volatile char&>((((s *)1)->m)) - 1)

TEST_F(ContextMemberTest, VerifyDetectedMembers)
{
  AutoRequired<HasAFewSlots> hasAFew;

  (void) auto_id_t_init<SimpleObject>::init;
  (void) auto_id_t_init<SimpleThreadedT<long>>::init;
  (void) auto_id_t_init<SimpleThreadedT<int>>::init;

  // Slots defined in reverse order here, because that's how they will be present in the collection
  SlotInformation expected [] = {
    {nullptr, auto_id_t<SimpleObject>{}, offsetof_nowarn(HasAFewSlots, m_sobj), false},
    {nullptr, auto_id_t<SimpleThreadedT<long>>{}, offsetof_nowarn(HasAFewSlots, m_sthread2), false},
    {nullptr, auto_id_t<SimpleThreadedT<int>>{}, offsetof_nowarn(HasAFewSlots, m_sthread1), false}
  };

  ASSERT_EQ(typeid(SimpleObject), *expected[0].type.block->ti);
  ASSERT_EQ(typeid(SimpleThreadedT<long>), *expected[1].type.block->ti);
  ASSERT_EQ(typeid(SimpleThreadedT<int>), *expected[2].type.block->ti);

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

struct RefersToTweedleDee;
struct RefersToTweedleDum;

struct RefersToTweedleDee {
  Autowired<RefersToTweedleDum> td;
};
struct RefersToTweedleDum {
  Autowired<RefersToTweedleDee> td;
};

TEST_F(ContextMemberTest, SimpleResetCase) {
  Autowired<CoreThread>().reset();
}

TEST_F(ContextMemberTest, CanStillObtainWeakPointer) {
  std::weak_ptr<SimpleObject> cm;

  {
    // We should be able to get a weak pointer from an Autowired field
    AutoCreateContext ctxt;
    ctxt->Inject<SimpleObject>();
    cm = Autowired<SimpleObject>(ctxt);
  }

  // Now verify that the weak pointer expired properly
  ASSERT_TRUE(cm.expired()) << "Weak pointer to an autowired slot did not reset as expected";
}

TEST_F(ContextMemberTest, ComplexResetCase) {
  std::weak_ptr<CoreContext> ctxtWeak;
  std::weak_ptr<RefersToTweedleDee> deeWeak;
  std::weak_ptr<RefersToTweedleDum> dumWeak;

  {
    // Set up the cycle:
    AutoCreateContext ctxt;
    ctxtWeak = ctxt;
    ctxt->Inject<RefersToTweedleDee>();
    ctxt->Inject<RefersToTweedleDum>();

    // Now try to reset the cycle:
    Autowired<RefersToTweedleDee> dee(ctxt);
    Autowired<RefersToTweedleDum> dum(ctxt);
    dee->td.reset();

    // Get weak pointers so we can verify that everything cleaned up
    deeWeak = dee;
    dumWeak = dum;
  }

  // Context should have gone away by now
  ASSERT_TRUE(ctxtWeak.expired()) << "Context was leaked even after a local cycle was reset";
  ASSERT_TRUE(dumWeak.expired()) << "Leak detected of a member that was explicitly released";
  ASSERT_TRUE(deeWeak.expired()) << "Leak detected of a member that was not explicitly released";
}

namespace {
  class TypeThatIsNotInjected {};
}

TEST_F(ContextMemberTest, PathologicalResetCase) {
  Autowired<TypeThatIsNotInjected>* pv;
  volatile std::atomic<size_t> nBarr{ 0 };
  volatile bool proceed = true;
  volatile bool go = false;

  auto resetsV = [&] {
    while(proceed) {
      ++nBarr;
      while (proceed && !go);
      if (!proceed)
        break;

      pv->reset();
      --nBarr;
      while (proceed && go);
    }
  };

  std::thread a{ resetsV };
  std::thread b{ resetsV };

  for (size_t i = 1000; i--;) {
    Autowired<TypeThatIsNotInjected> v;
    pv = &v;
    for (size_t j = 10; j--;)
      v.NotifyWhenAutowired([] {});

    // Bump the threads to spin around:
    while (nBarr != 2);
    go = true;
    while (nBarr);
    go = false;
  }

  proceed = false;
  a.join();
  b.join();
}
