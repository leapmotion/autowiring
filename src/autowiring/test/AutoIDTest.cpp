// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"
#include <autowiring/autowiring.h>

class AutoIDTest:
  public testing::Test
{};

TEST_F(AutoIDTest, VerifyIndexUniqueness) {
  const autowiring::auto_id_block* v0 = &auto_id_t<Decoration<0>>::s_block;
  const autowiring::auto_id_block* v1 = &auto_id_t<Decoration<1>>::s_block;

  ASSERT_NE(nullptr, v0);
  ASSERT_NE(nullptr, v1);
  ASSERT_NE(v0, v1) << "Indexes were equal when they should have been distinct";
  ASSERT_NE(*v0, *v1) << "Blocks at distinct addresses incorrectly evaluated as being equal";
}
