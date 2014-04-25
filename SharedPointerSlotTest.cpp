#include "stdafx.h"
#include "SharedPointerSlotTest.h"
#include "SharedPointerSlot.h"

class MyUnusedClass {};

template<>
struct SharedPointerSlotT<MyUnusedClass>:
  SharedPointerSlot
{
  SharedPointerSlotT(const std::shared_ptr<MyUnusedClass>& rhs):
    dtorStrike(false)
  {}

  ~SharedPointerSlotT(void) {
    dtorStrike = true;
  }

  bool dtorStrike;
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
  ASSERT_FALSE(mucSlot.dtorStrike) << "Destructor was struck prematurely";

  // Direct destructor call:
  slot.~SharedPointerSlot();

  // Verify we hit our dtor in the specialization we declared above:
  ASSERT_TRUE(mucSlot.dtorStrike) << "Virtual destructor on in-place polymorphic class was not hit as expected";
}

TEST_F(SharedPointerSlotTest, SlotReassignment) {
  std::shared_ptr<bool> sharedPointerA(new bool);
  std::shared_ptr<bool> sharedPointerB(new bool);
  std::shared_ptr<int> sharedPointerC(new int);

  // Make our slot, and start off with a shared pointer of one type:
  AnySharedPointer slot;
  slot = sharedPointerA;

  // Recast to another shared pointer, verify reference count goes down:
  slot = sharedPointerB;
  ASSERT_TRUE(sharedPointerA.unique()) << "Destructor was not properly invoked for a shared pointer slot";
  ASSERT_EQ(2UL, sharedPointerB.use_count()) << "Reference count was not incremented properly during a shared pointer hold";

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
      slots.push_back(sharedPtr);
      ASSERT_EQ(1 + slots.size(), sharedPtr.use_count()) << "Unexpected number of references to a slotted shared pointer";
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
    AnySharedPointer slot1 = sharedPtr;
    ASSERT_FALSE(slot1->empty()) << "A slot initialized from a shared pointer was incorrectly marked as empty";

    // Verify the type came across:
    EXPECT_EQ(typeid(bool), slot1->type()) << "Dynamic initialization did not correctly adjust the dynamic type";

    // Now copy it over:
    slot2 = slot1;

    // Verify reference count was affected as expected
    ASSERT_EQ(3UL, sharedPtr.use_count()) << "A second-order slot copy didn't increment the reference count as expected";
  }

  // Verify that the slot still holds a reference and that the reference count is correct:
  ASSERT_FALSE(slot2->empty()) << "A slot should have continued to hold a shared pointer, but was prematurely cleared";
  ASSERT_EQ(2UL, sharedPtr.use_count()) << "A slot going out of scope did not correctly decrement a shared pointer reference";
}