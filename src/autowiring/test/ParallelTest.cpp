// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "gtest-all-guard.hpp"
#include <autowiring/Parallel.h>

class ParallelTest:
  public testing::Test
{};

TEST_F(ParallelTest, Basic) {
  AutoCurrentContext()->Initiate();
  autowiring::parallel p;

  for (int i : {0,4,2,5,1,3}) {
    p += [i]() {
      return i;
    };
  }

  std::vector<int> result;
  for (auto it = p.begin<int>(); it != p.end<int>(); ++it) {
    result.push_back(*it);
  }

  ASSERT_EQ(result.size(), 6) << "Didn't receive all value";

  std::sort(result.begin(), result.end());
  for (int i = 0; i < static_cast<int>(result.size()); ++i) {
    ASSERT_EQ(i, result[i]) << "Didn't receive correct values";
  }
}