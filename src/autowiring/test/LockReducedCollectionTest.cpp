// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "LockReducedCollectionTest.h"
#include "LockReducedCollection.h"
#include <boost/thread/barrier.hpp>
#include <boost/thread/thread.hpp>

struct IntHash {
  size_t operator()(int v) const {
    return v;
  }

  bool operator()(int lh, int rh) const {
    return lh == rh;
  }
};

TEST_F(LockReducedCollectionTest, ClearCheck) {
  LockReducedCollection<int, IntHash> collection;
  collection.Insert(10);

  // Try to clear the collection and verify the happens-after ordering:
  collection.Clear();
  auto value = collection.GetImage();
  ASSERT_TRUE(!!value) << "Collection image was empty, this is a fatal condition";
  EXPECT_TRUE(value->empty()) << "Collection contained elements even after it was cleared";
}

TEST_F(LockReducedCollectionTest, SimpleMembershipCheck) {
  // Verify trivial insertion works as we expect.
  LockReducedCollection<int, IntHash> collection;

  // Note that each of these operations are O(n).  Ordinarly, sequential
  // insertion like this in large scales would make a good case to optimize
  // this function.
  collection.Insert(10);
  collection.Insert(11);
  collection.Insert(12);

  // Size check:
  auto image = collection.GetImage();
  EXPECT_EQ(image->size(), static_cast<size_t>(3)) << "Unexpected collection count";
}

TEST_F(LockReducedCollectionTest, ConcurrentWritersCheck) {
  LockReducedCollection<int, IntHash> collection;

  const size_t threadCount = 10;
  boost::barrier barrier(threadCount);

  // VERY INEFFICIENT, but should put the collection through its paces
  boost::thread allThreads[threadCount];
  for(size_t i = 0; i < threadCount; i++)
    allThreads[i] = boost::thread([&collection, &barrier, i] () {
      barrier.wait();
      collection.Insert((int)i);
    });

  // Wait on all threads:
  for(size_t i = 0; i < threadCount; i++)
    if (allThreads[i].joinable())
      allThreads[i].join();

  // Trivial size validation first:
  auto image = collection.GetImage();
  ASSERT_EQ(threadCount, image->size()) << "Total number of elements inserted in parallel was less than expected";

  // Verify that all numbers in the expected range exist:
  for(size_t i = 0; i < threadCount; i++)
    ASSERT_EQ(image->count((int)i), static_cast<size_t>(1)) << "Element " << i << " was missing from the collection";
}
