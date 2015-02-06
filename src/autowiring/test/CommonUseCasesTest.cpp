// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "HasForwardOnlyType.hpp"

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
