// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"
#include <autowiring/CreationRules.h>

class CreationRulesTest:
  public testing::Test
{};

struct AlignedFreer {
  void operator()(void* pMem) const {
    autowiring::aligned_free(pMem);
  }
};

TEST_F(CreationRulesTest, AlignedAllocation) {
  // Use a vector because we don't want the underlying allocator to give us the same memory block
  // over and over--we are, after all, freeing memory and then immediately asking for another block
  // with identical size requirements.
  std::vector<std::unique_ptr<void, AlignedFreer>> memory;
  memory.resize(100);

  // Allocate one byte of 256-byte aligned memory 100 times.  If the aligned allocator isn't working
  // properly, then the odds are good that at least one of these allocations will be wrong.
  for (auto& ptr : memory) {
    ptr.reset(autowiring::aligned_malloc(1, 256));
    ASSERT_EQ(0UL, (size_t)ptr.get() % 256) << "Aligned allocator did not return a correctly aligned pointer";
  }
}
