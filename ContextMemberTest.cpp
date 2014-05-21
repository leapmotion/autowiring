#include "stdafx.h"
#include "ContextMemberTest.h"
#include "TestFixtures/SimpleObject.h"
#include "TestFixtures/SimpleThreaded.h"

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

TEST_F(ContextMemberTest, VerifyDetectedMembers)
{
  AutoRequired<HasAFewSlots> hasAFew;

  // Slots defined in reverse order here, because that's how they will be present in the collection
  SlotInformation expected [] = {
    SlotInformation(nullptr, typeid(SimpleObject), offsetof(HasAFewSlots, m_sobj), false),
    SlotInformation(nullptr, typeid(SimpleThreadedT<long>), offsetof(HasAFewSlots, m_sthread2), false),
    SlotInformation(nullptr, typeid(SimpleThreadedT<int>), offsetof(HasAFewSlots, m_sthread1), false)
  };

  // Validate all pointers are what we expect to find, and in the right order
  size_t ct = 0;
  for(auto cur = hasAFew->GetSlotInformation(); cur; cur = cur->pFlink) {
    // Validate slot types:
    ASSERT_EQ(expected[ct].type, cur->type) << "Slot number " << ct << " had a mismatched type";
    ASSERT_EQ(expected[ct].slotOffset, cur->slotOffset) << "Slot number " << ct << " had a mismatched offset";
    ct++;
  }
  ASSERT_EQ(3UL, ct) << "Slot registration detected an insufficient number of created slots";
}
