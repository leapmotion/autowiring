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
  SharedPointerSlot slot;
  slot = sharedPointerA;

  // Recast to another shared pointer, verify reference count goes down:
  slot = sharedPointerB;
  ASSERT_TRUE(sharedPointerA.unique()) << "Destructor was not properly invoked for a shared pointer slot";
}
