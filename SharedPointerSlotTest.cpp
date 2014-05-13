#include "stdafx.h"
#include "SharedPointerSlotTest.h"
#include "SharedPointerSlot.h"

class MyUnusedClass {};

template<>
struct SharedPointerSlotT<MyUnusedClass>:
  SharedPointerSlot
{
  SharedPointerSlotT(const std::shared_ptr<MyUnusedClass>& rhs)
  {
    dtorStrike() = false;
  }

  ~SharedPointerSlotT(void) {
    dtorStrike() = true;
  }

  bool& dtorStrike(void) {
    return (bool&) *m_space;
  }
};

TEST_F(SharedPointerSlotTest, SimpleDestructorStrike)
{
  // We will need a buffer big enough for entire slot:
  unsigned char buf[sizeof(SharedPointerSlotT<MyUnusedClass>)];
  auto& mucSlot = *(SharedPointerSlotT<MyUnusedClass>*)buf;

  // Placement new a basic shared pointer slot in the buffer:
  SharedPointerSlot& slot = *new(buf) SharedPointerSlot;

  // In-place polymorphism on the slot:
  std::shared_ptr<MyUnusedClass> unused(new MyUnusedClass);
  slot = unused;

  // Destructor shouldn't be hit until we call it:
  ASSERT_FALSE(mucSlot.dtorStrike()) << "Destructor was struck prematurely";

  // Direct destructor call:
  slot.~SharedPointerSlot();

  // Verify we hit our dtor in the specialization we declared above:
  ASSERT_TRUE(mucSlot.dtorStrike()) << "Virtual destructor on in-place polymorphic class was not hit as expected";
}

TEST_F(SharedPointerSlotTest, AnySharedPointerRelease) {
  auto t = std::make_shared<int>(5);

  // Assign over, then reset
  {
    AnySharedPointer ptr;
    ptr = t;
  }

  // Verify the AnySharedPointer destructor worked correctly
  ASSERT_TRUE(t.unique()) << "AnySharedPointer instance did not properly release a reference when destroyed";
}

TEST_F(SharedPointerSlotTest, SlotReassignment) {
  std::shared_ptr<bool> sharedPointerA(new bool);
  std::shared_ptr<bool> sharedPointerB(new bool);
  std::shared_ptr<int> sharedPointerC(new int);

  // Make our slot, and start off with a shared pointer of one type:
  AnySharedPointer slot;
  slot = sharedPointerA;

  // Verify that the assignment worked as anticipated:
  ASSERT_EQ(2, sharedPointerA.use_count()) << "Constructor did not properly addref a shared pointer on initialization";

  // Recast to another shared pointer, verify reference count goes down:
  slot = sharedPointerB;
  EXPECT_EQ(2, sharedPointerB.use_count()) << "Reference count was not incremented properly during a shared pointer hold";
  ASSERT_TRUE(sharedPointerA.unique()) << "Destructor was not properly invoked for a shared pointer slot";

  // Now change the type completely, verify proper release:
  slot = sharedPointerC;
  ASSERT_TRUE(sharedPointerB.unique()) << "Second assigned shared pointer was not released under type transformation";
}

TEST_F(SharedPointerSlotTest, SlotsInVector) {
  std::shared_ptr<bool> sharedPtr(new bool);

  {
    std::vector<AnySharedPointer> slots;

    // Initialize with a lot of copies of sharedPtr
    for(size_t i = 0; i < 10; i++) {
      slots.push_back(AnySharedPointer(sharedPtr));
      ASSERT_EQ(1 + (long)slots.size(), sharedPtr.use_count()) << "Unexpected number of references to a slotted shared pointer";
    }
  }

  // Now verify that we're unique again:
  ASSERT_TRUE(sharedPtr.unique()) << "Slots in a vector did not properly release all local clones";
}

TEST_F(SharedPointerSlotTest, SlotDuplication) {
  std::shared_ptr<bool> sharedPtr(new bool);

  AnySharedPointer slot2;

  {
    // Create a base slot to hold the shared pointer:
    AnySharedPointer slot1(sharedPtr);
    ASSERT_FALSE(slot1->empty()) << "A slot initialized from a shared pointer was incorrectly marked as empty";

    // Verify the type came across:
    EXPECT_EQ(typeid(bool), slot1->type()) << "Dynamic initialization did not correctly adjust the dynamic type";

    // Now copy it over:
    slot2 = slot1;

    // Verify reference count was affected as expected
    ASSERT_EQ(3, sharedPtr.use_count()) << "A second-order slot copy didn't increment the reference count as expected";
  }

  // Verify that the slot still holds a reference and that the reference count is correct:
  ASSERT_FALSE(slot2->empty()) << "A slot should have continued to hold a shared pointer, but was prematurely cleared";
  ASSERT_EQ(2, sharedPtr.use_count()) << "A slot going out of scope did not correctly decrement a shared pointer reference";
}

TEST_F(SharedPointerSlotTest, TrivialRelease) {
  std::shared_ptr<bool> a(new bool);
  std::shared_ptr<int> b(new int);

  // Assign the slot to two different values, and make sure that they are released properly
  AnySharedPointer slot;
  slot = a;
  slot = b;

  ASSERT_TRUE(a.unique()) << "Expected reassignment of a slot to release a formerly held instance";
  ASSERT_FALSE(b.unique()) << "Expected slot to hold a reference to the second specified instance";
  
  // Now release, and verify that a release actually took place
  slot->reset();
  ASSERT_TRUE(b.unique()) << "Releasing a slot did not actually release the held value as expected";
}

TEST_F(SharedPointerSlotTest, InitDerivesCorrectType) {
  AnySharedPointer slot;
  slot->init<int>();

  ASSERT_EQ(typeid(int), slot->type()) << "A manually initialized slot did not have the expected type";
}

TEST_F(SharedPointerSlotTest, VoidReturnExpected) {
  // Fill out our slot:
  auto v = std::make_shared<int>(5);
  AnySharedPointer slot;
  slot = v;

  // Validate equivalence of the void operator:
  ASSERT_EQ(v.get(), slot->ptr()) << "Shared pointer slot did not return a void* with an expected value";
}
