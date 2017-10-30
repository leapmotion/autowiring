// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"
#include "TestFixtures/SimpleObject.hpp"
#include <autowiring/AnySharedPointer.h>
#include <autowiring/autowiring.h>

class AnySharedPointerTest:
  public testing::Test
{};

// Autowiring takes advantage of the representational equivalence of instantiations of
// shared_ptr, and therefore requires that their layouts all be the same.
static_assert(
  sizeof(std::shared_ptr<void*>) == sizeof(std::shared_ptr<int>),
  "Shared pointer representations must be equivalent for casting to work correctly"
);

TEST_F(AnySharedPointerTest, CanReinterpretCastSharedPtr) {
  auto iShared = std::make_shared<int>(101);
  auto vShared = *reinterpret_cast<std::shared_ptr<void>*>(&iShared);

  ASSERT_EQ(2UL, iShared.use_count()) << "Reinterpretive assignment of a void pointer did not increment the use count";
  ASSERT_EQ(iShared.get(), vShared.get()) << "Reinterpreted shared pointer did not properly hold a member variable";
}

class MyUnusedClass {};

TEST_F(AnySharedPointerTest, OperatorEq) {
  AutoRequired<SimpleObject> sobj;

  // Trivial equivalence of an AnySharedPointer based on an AutoFilter instance
  AnySharedPointer sobjAny1(sobj);
  ASSERT_TRUE((bool)sobjAny1);
  ASSERT_EQ(sobj, sobjAny1) << "An AnySharedPointer instance initialized by constructor violated an identity test";

  // Trivial equivalence of an AnySharedPointer based on an AutoFilter instance
  AnySharedPointer sobjAny2;
  sobjAny2 = sobj;
  ASSERT_TRUE((bool)sobjAny2);
  ASSERT_EQ(sobj, sobjAny2) << "An AnySharedPointer instance initialized by assignment violated an identity test";
}

TEST_F(AnySharedPointerTest, AnySharedPointerRelease) {
  auto t = std::make_shared<int>(5);

  // Assign over, then reset
  {
    AnySharedPointer ptr;
    ptr = t;
  }

  // Verify the AnySharedPointer destructor worked correctly
  ASSERT_TRUE(t.unique()) << "AnySharedPointer instance did not properly release a reference when destroyed";
}

TEST_F(AnySharedPointerTest, SlotReassignment) {
  auto sharedPointerA = std::make_shared<bool>();
  auto sharedPointerB = std::make_shared<bool>();
  auto sharedPointerC = std::make_shared<int>();

  // Make our slot, and start off with a shared pointer of one type:
  AnySharedPointer slot;
  slot = sharedPointerA;

  // Verify that the assignment worked as anticipated:
  ASSERT_EQ(2, sharedPointerA.use_count()) << "Constructor did not properly addref a shared pointer on initialization";

  // Recast to another shared pointer, verify reference count goes down:
  slot = sharedPointerB;
  ASSERT_EQ(2, sharedPointerB.use_count()) << "Reference count was not incremented properly during a shared pointer hold";
  ASSERT_TRUE(sharedPointerA.unique()) << "Destructor was not properly invoked for a shared pointer slot";

  // Now change the type completely, verify proper release:
  slot = sharedPointerC;
  ASSERT_TRUE(sharedPointerB.unique()) << "Second assigned shared pointer was not released under type transformation";
}

TEST_F(AnySharedPointerTest, SlotsInVector) {
  auto sharedPtr = std::make_shared<bool>();

  {
    std::list<AnySharedPointer> slots;

    // Initialize with a lot of copies of sharedPtr
    for(size_t i = 0; i < 10; i++) {
      slots.push_back(AnySharedPointer(sharedPtr));
      ASSERT_EQ(1 + (long)slots.size(), sharedPtr.use_count()) << "Unexpected number of references to a slotted shared pointer";
    }
  }

  // Now verify that we're unique again:
  ASSERT_TRUE(sharedPtr.unique()) << "Slots in a vector did not properly release all local clones";
}

TEST_F(AnySharedPointerTest, SlotDuplication) {
  auto sharedPtr = std::make_shared<bool>();

  AnySharedPointer slot2;

  {
    // Create a base slot to hold the shared pointer:
    AnySharedPointer slot1(sharedPtr);
    ASSERT_FALSE(slot1.empty()) << "A slot initialized from a shared pointer was incorrectly marked as empty";

    // Verify the type came across:
    ASSERT_EQ(auto_id_t<bool>{}, slot1.type()) << "Dynamic initialization did not correctly adjust the dynamic type";

    // Now copy it over:
    slot2 = slot1;

    // Verify reference count was affected as expected
    ASSERT_EQ(3, sharedPtr.use_count()) << "A second-order slot copy didn't increment the reference count as expected";
  }

  // Verify that the slot still holds a reference and that the reference count is correct:
  ASSERT_FALSE(slot2.empty()) << "A slot should have continued to hold a shared pointer, but was prematurely cleared";
  ASSERT_EQ(2, sharedPtr.use_count()) << "A slot going out of scope did not correctly decrement a shared pointer reference";
}

TEST_F(AnySharedPointerTest, TrivialRelease) {
  auto a = std::make_shared<bool>();
  auto b = std::make_shared<int>();

  // Assign the slot to two different values, and make sure that they are released properly
  AnySharedPointer slot;
  slot = a;
  slot = b;

  ASSERT_TRUE(a.unique()) << "Expected reassignment of a slot to release a formerly held instance";
  ASSERT_FALSE(b.unique()) << "Expected slot to hold a reference to the second specified instance";

  // Now release, and verify that a release actually took place
  slot.reset();
  ASSERT_TRUE(b.unique()) << "Releasing a slot did not actually release the held value as expected";
}

TEST_F(AnySharedPointerTest, NoMultipleDelete) {
  auto a = std::make_shared<bool>();
  std::weak_ptr<bool> b = a;

  // Create a slot and validate the behavior of reset, and to ensure that the underlying
  // SharedPointerSlot is not multiply deleted.
  {
    AnySharedPointer slot;
    slot = a;
    slot.reset();
  }

  // Now verify that we didn't accidentally overdecrement the count:
  ASSERT_FALSE(b.expired()) << "Shared pointer prematurely expired; SharedPointerSlot dtor double-strike suspected";
}

TEST_F(AnySharedPointerTest, InitDerivesCorrectType) {
  AnySharedPointer slot;
  slot.init<int>();

  ASSERT_EQ(auto_id_t<int>{}, slot.type()) << "A manually initialized slot did not have the expected type";
}

TEST_F(AnySharedPointerTest, VoidReturnExpected) {
  // Fill out our slot:
  auto v = std::make_shared<int>(5);
  AnySharedPointer slot;
  slot = v;

  // Validate equivalence of the void operator:
  ASSERT_EQ(v.get(), slot.ptr()) << "Shared pointer slot did not return a void* with an expected value";
}

TEST_F(AnySharedPointerTest, CanHoldCoreObject) {
  auto co = std::make_shared<CoreObject>();

  AnySharedPointer x = co;
  ASSERT_EQ(co, x) << "Held CoreObject was not equivalent to constructed instance";
}

TEST_F(AnySharedPointerTest, CanFastCastToSelf) {
  (void)autowiring::fast_pointer_cast_initializer<CoreObject, CoreObject>::sc_init;

  auto co = std::make_shared<CoreObject>();
  ASSERT_EQ(
    co,
    autowiring::fast_pointer_cast<CoreObject>(co)
  ) << "Could not cast a CoreObject instance to itself";
}

class AlternateBase {
public:
  virtual ~AlternateBase(void) {}
};

class AnySharedPtrObjA:
  public CoreObject
{
public:
  int aVal = 101;
  int aVal2 = 101;
};

class AnySharedPtrObjB:
  public AlternateBase,
  public Decoration<0>,
  public AnySharedPtrObjA
{
public:
  int bVal = 102;
};

TEST_F(AnySharedPointerTest, CanCrossCast) {
  (void) auto_id_t_init<AnySharedPtrObjA>::init;
  (void) auto_id_t_init<AnySharedPtrObjB>::init;

  // Ensure that dynamic casters are non-null in the block:
  auto nullCaster = &autowiring::null_cast<AnySharedPtrObjA, CoreObject>;
  ASSERT_NE(
    nullCaster,
    (autowiring::fast_pointer_cast_blind<AnySharedPtrObjA, CoreObject>::cast)
  ) << "Fast pointer caster for AnySharedPtrObjA was not correctly initialized";
  ASSERT_NE(
    reinterpret_cast<std::shared_ptr<void>(*)(const std::shared_ptr<CoreObject>&)>(nullCaster),
    auto_id_t<AnySharedPtrObjA>::s_block.pFromObj
  ) << "AnySharedPtrObjA dynamic caster was incorrectly assigned";

  auto rootB = std::make_shared<AnySharedPtrObjB>();
  auto rootA = std::static_pointer_cast<AnySharedPtrObjA>(rootB);
  auto rootBPtr = rootB.get();
  auto rootAPtr = static_cast<AnySharedPtrObjA*>(rootB.get());
  ASSERT_NE((void*) rootBPtr, (void*) rootAPtr) << "Objects were not correctly detected as having separate offsets";

  AnySharedPointer aASP = rootA;
  std::shared_ptr<CoreObject> obj = aASP.as_obj();
  ASSERT_NE(nullptr, obj) << "An object cast attempt did not succeed as expected";
  ASSERT_EQ(101, aASP.as<AnySharedPtrObjA>()->aVal);

  AnySharedPointer bASP;
  bASP.init<AnySharedPtrObjB>();
  bASP.try_assign(obj);

  ASSERT_NE(bASP, nullptr) << "An attempted cast incorrectly resulted in a null return";
  ASSERT_EQ(102, bASP.as<AnySharedPtrObjB>()->bVal);
  ASSERT_EQ(aASP, bASP) << "An aliased shared pointer was not detected as being equal";
}

TEST_F(AnySharedPointerTest, NullAfterMove) {
  auto ptr = std::make_shared<bool>(false);
  AnySharedPointer p1 = ptr;
  AnySharedPointer p2 = std::move(p1);
  ASSERT_FALSE(p1) << "Move construction of AnySharedPointer did not nullify rhs";
  AnySharedPointer p3;
  p3 = std::move(p2);
  ASSERT_FALSE(p2) << "Move assignment of AnySharedPointer did not nullify rhs";
}

TEST_F(AnySharedPointerTest, NullPtrConstruction) {
  AnySharedPointer x{ nullptr };
  ASSERT_EQ(auto_id_t<void>{}, auto_id{}) << "Default auto_id is expected to be void";
  ASSERT_EQ(auto_id_t<void>{}, x.type()) << "nullptr type_id should have been void";

  AnySharedPointer y;
  AnySharedPointer z;
  y = nullptr;

  ASSERT_EQ(x, y) << "Nullptr initialization was not null";
  ASSERT_EQ(x, z) << "Nullptr assignment was not null";
  ASSERT_EQ(auto_id_t<void>{}, x.type());
  ASSERT_EQ(auto_id_t<void>{}, y.type());
  ASSERT_EQ(auto_id_t<void>{}, z.type());
}

TEST_F(AnySharedPointerTest, VoidSharedPointer) {
  auto p = std::make_shared<int>(101);
  AnySharedPointer x = p;
  ASSERT_EQ(std::static_pointer_cast<void>(p), x.as_void()) << "Void cast of shared pointer did not hold the expected value";
}
