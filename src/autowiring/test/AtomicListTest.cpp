// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/at_exit.h>
#include <autowiring/atomic_list.h>
#include <thread>

using autowiring::atomic_list;

struct HoldsInt :
  autowiring::callable_base
{
  HoldsInt(int value) : value(value) {}
  int value;
  operator int(void) const { return value; }
};

TEST(AtomicListTest, SimpleInsertion) {
  atomic_list l;
  uint32_t ids[] = {
    l.push<HoldsInt>(55),
    l.push<HoldsInt>(56),
    l.push<HoldsInt>(57)
  };
  ASSERT_EQ(ids[0], ids[1]);
  ASSERT_EQ(ids[0], ids[2]);

  auto e = l.release<HoldsInt>();
  auto q = e.begin();

  for (size_t i = 55; i <= 57; i++) {
    ASSERT_NE(e.end(), q) << "Unexpected end of list encountered";
    EXPECT_EQ(i, *q) << "List retrieved in the wrong order";
    ++q;
  }
}

TEST(AtomicListTest, IdIncrement) {
  atomic_list l;
  uint32_t id1 = l.push<HoldsInt>(101);
  l.release<HoldsInt>();
  uint32_t id2 = l.push<HoldsInt>(102);
  ASSERT_NE(id1, id2) << "Identifier not incremented as expected";
}

TEST(AtomicListTest, IdIncrementPathological) {
  enum class Owner { None, Consumer, Producer };

  atomic_list l;
  std::atomic<int> latest{ -1 };
  std::atomic<uint32_t> lastChain{ ~0U };
  volatile bool proceed = true;

  // Initial owner is the consumer:
  std::atomic<Owner> owner{ Owner::Consumer };

  std::thread consumer([&] {
    owner = Owner::None;

    while (proceed) {
      Owner exp;
      do exp = Owner::None;
      while(!owner.compare_exchange_weak(exp, Owner::Consumer));
      auto x = MakeAtExit([&owner] { owner = Owner::None; });

      lastChain = l.chain_id();
      auto f = l.release<HoldsInt>();
      for (int value : f)
        latest = value;
    }
  });
  auto x = MakeAtExit([&] {
    proceed = false;
    consumer.join();
  });

  // Drive items into the list and see if we can rip them back:
  for (int i = 0; i < 10000; i++) {
    uint32_t insertedID = l.push<HoldsInt>(i);

    Owner exp;
    do exp = Owner::None;
    while (!owner.compare_exchange_weak(exp, Owner::Producer));
    auto x = MakeAtExit([&owner] { owner = Owner::None; });

    auto curChainID = l.chain_id();
    if (curChainID == insertedID) {
      // Consumer must not have reported that it saw this ID
      ASSERT_NE(lastChain, curChainID) << "Consumer reported that it saw a chain ID that should still be open";

      // Still in the list, should not have been observed by the consumer yet
      ASSERT_LT(latest, i) << "Chain ID was not updated, but element was found in consumer set";
    }
    else
      // Not in the list, must be in the set
      ASSERT_GE(latest, i) << "Chain ID was updated, but element was not found in consumer set";
  }
}
