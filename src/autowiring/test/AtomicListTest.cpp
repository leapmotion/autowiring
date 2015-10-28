// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "at_exit.h"
#include "atomic_list.h"
#include <thread>
#include <unordered_set>

using autowiring::atomic_list;

TEST(AtomicListTest, SimpleInsertion) {
  atomic_list<int> l;
  uint32_t ids[] = {
    l.push(55),
    l.push(56),
    l.push(57)
  };
  ASSERT_EQ(ids[0], ids[1]);
  ASSERT_EQ(ids[0], ids[2]);

  auto e = l.release();
  auto q = e.begin();

  for (size_t i = 55; i <= 57; i++) {
    ASSERT_NE(e.end(), q) << "Unexpected end of list encountered";
    EXPECT_EQ(i, *q) << "List retrieved in the wrong order";
    ++q;
  }
}

TEST(AtomicListTest, IdIncrement) {
  atomic_list<int> l;
  uint32_t id1 = l.push(101);
  auto e = l.release();
  uint32_t id2 = l.push(102);
  ASSERT_NE(id1, id2) << "Identifier not incremented as expected";
}

TEST(AtomicListTest, IdIncrementPathological) {
  enum class Owner { None, Consumer, Producer };

  atomic_list<int> l;
  std::unordered_set<int> S;
  volatile bool proceed = false;

  // Initial owner is the consumer:
  std::atomic<Owner> owner{ Owner::Consumer };

  std::thread consumer([&] {
    owner = Owner::None;

    while (proceed) {
      Owner exp;
      do exp = Owner::None;
      while(!owner.compare_exchange_weak(exp, Owner::Consumer));
      auto x = MakeAtExit([&owner] { owner = Owner::None; });

      auto f = l.release();
      for(int value : f)
        S.insert(value);
    }
  });
  auto x = MakeAtExit([&] {
    proceed = false;
    consumer.join();
  });

  // Drive items into the list and see if we can rip them back:
  for (size_t i = 10000; i--;) {
    uint32_t insertedID = l.push(i);

    Owner exp;
    do exp = Owner::None;
    while (!owner.compare_exchange_weak(exp, Owner::Producer));
    auto x = MakeAtExit([&owner] { owner = Owner::None; });

    auto curChainID = l.chain_id();
    if (curChainID == insertedID)
      // Still in the list, should not have been observed by the consumer yet
      ASSERT_EQ(0UL, S.count(i)) << "Chain ID was not updated, but element was found in consumer set";
    else
      // Not in the list, must be in the set
      ASSERT_EQ(1UL, S.count(i)) << "Chain ID was updated, but element was not found in consumer set";
  }
}
