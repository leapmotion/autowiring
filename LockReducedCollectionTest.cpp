#include "stdafx.h"
#include "LockReducedCollectionTest.h"
#include "LockReducedCollection.h"
#include <boost/thread.hpp>

struct IntHash {
  size_t operator()(int v) const {
    return v;
  }

  bool operator()(int lh, int rh) const {
    return lh == rh;
  }
};

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
  EXPECT_EQ(image->size(), 3) << "Unexpected collection count";
}

TEST_F(LockReducedCollectionTest, ConcurrentWritersCheck) {
  LockReducedCollection<int, IntHash> collection;

  const size_t threadCount = 100;
  boost::thread threads[threadCount];

  // VERY INEFFICIENT, but should put the collection through its paces
  for(int i = 0; i < threadCount; i++)
    boost::thread([&collection, i] () {
      collection.Insert(i);
    });

  // Wait on all threads:
  for(size_t i = 0; i < threadCount; i++)
    threads[i].join();

  // Trivial size validation first:
  auto image = collection.GetImage();
  ASSERT_EQ(threadCount, image->size()) << "Total number of elements inserted in parallel was less than expected";

  // Verify that all numbers in the expected range exist:
  for(size_t i = 0; i < threadCount; i++)
    ASSERT_EQ(image->count(i), 1) << "Element " << i << " was missing from the collection";
}