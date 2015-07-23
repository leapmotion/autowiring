// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "HasForwardOnlyType.hpp"
#include <autowiring/is_complete.h>

struct MyIncompleteType;
static_assert(!AUTO_IS_COMPLETE(MyIncompleteType), "Type known to be incomplete was marked complete");
static_assert(AUTO_IS_COMPLETE(std::mutex), "Type known to be complete was marked incomplete");

// Now provide the definition of our formerly incomplete type and verify that we get updated
struct MyIncompleteType {
  MyIncompleteType() = delete;
  MyIncompleteType(const MyIncompleteType&) = delete;
};
static_assert(AUTO_IS_COMPLETE(MyIncompleteType), "Type that was completed later was not identified as having been completed");

class CommonUseCasesTest:
  public testing::Test
{};

class CUCTSample {};

// This file contains common use cases that have been observed often in the wild,
// and are therefore patterns that must be supported.  Adjustment of this file should
// only take place with great attention paid to a proper deprecation strategy.
TEST_F(CommonUseCasesTest, AutowiredForwardType) {
  HasForwardOnlyType f;
  AutoRequired<HasForwardOnlyType> fp;
}
