// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/atomic_object.h>
#include <autowiring/unlock_object.h>

class GuardObjectTest:
public testing::Test
{};

class copy_count {
  mutable int m_copy = 0;

public:
  copy_count() {}

  copy_count(const copy_count& source): m_copy(source.m_copy + 1) {
    source.m_copy = m_copy;
  }

  copy_count& operator = (const copy_count& source) {
    ++source.m_copy;
    m_copy = source.m_copy;
    return *this;
  }

  int count() {
    return m_copy;
  }
};

TEST_F(GuardObjectTest, AtomicTests) {
  copy_count add_count;
  atomic_object<copy_count> atomic(add_count); //atomic.count == add_count == 1
  ASSERT_EQ(1, add_count.count());

  //Verify casting to base type by copy
  add_count = atomic; //atomic.count == 2
  ASSERT_EQ(3, add_count.count());

  //Verify that deadlock in self-assignment is avoided
  atomic = atomic; //atomic.count == 2
  add_count = atomic; //atomic.count == 3
  ASSERT_EQ(4, add_count.count());

  //Verify that construction from atomic object does not deadlock
  atomic_object<copy_count> atomic_new(atomic); //atomic_new == atomic_count == 4
  add_count = atomic; //atomic.count == 5
  ASSERT_EQ(6, add_count.count());
}

TEST_F(GuardObjectTest, UnlockMemberTests) {
  atomic_object<copy_count>::shared counter(new atomic_object<copy_count>());
  copy_count add_count;

  //Verify unlock, assignment and re-lock
  {
    //PROBLEM: Existence guarantee is not possible for objects on stack!

    atomic_object<copy_count>::unlock unlock(counter);
    ASSERT_EQ(0, unlock->count());
    //NOTE: Implicit cast-by-copy made only one copy from the original

    //Test dereferencing member access
    *unlock = add_count;
    ASSERT_EQ(1, unlock->count());
  }

  //Verify unlock on destruction
  atomic_object<copy_count>::unlock try_unlock(counter, true);
  ASSERT_TRUE(try_unlock);
  try_unlock.release();
  ASSERT_FALSE(try_unlock);
}

TEST_F(GuardObjectTest, UnlockBlockingTests) {
  //Verify exclusion
  {
    atomic_object<int>::shared shared;

    //Verify that no reference is aquired
    atomic_object<int>::unlock unlock(shared);
    ASSERT_FALSE(unlock);

    shared.reset(new atomic_object<int>());
    unlock.acquire(shared);
    ASSERT_TRUE(unlock);
    ASSERT_EQ(2, shared.use_count());
    unlock.acquire(shared);
    ASSERT_TRUE(unlock);
    ASSERT_EQ(2, shared.use_count());

    //Verify that no reference is aquired
    atomic_object<int>::unlock try_unlock(shared, true);
    ASSERT_FALSE(try_unlock);
    ASSERT_EQ(2, shared.use_count());
  }

  //Verify existence guarantee
  atomic_object<int>::unlock keeper;
  {
    atomic_object<int>::shared shared(new atomic_object<int>(0));
    keeper.acquire(shared);
    ASSERT_TRUE(keeper);
    ASSERT_EQ(2, shared.use_count());
  }
  ASSERT_TRUE(keeper);
  *keeper = 1;
}
