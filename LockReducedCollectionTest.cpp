#include "stdafx.h"
#include "LockReducedCollectionTest.h"
#include "LockReducedCollection.h"

struct IntHash {
  size_t operator()(int v) const {
    return v;
  }

  bool operator()(int lh, int rh) const {
    return lh == rh;
  }
};

TEST_F(LockReducedCollectionTest, SimpleMembershipCheck) {
  LockReducedCollection<int, IntHash> collection;
  collection.Insert(10);
  collection.Insert(11);
  collection.Insert(12);

  // Size check:
  auto image = collection.GetImage();
  EXPECT_EQ(image->size(), 3) << "Unexpected collection count";
}