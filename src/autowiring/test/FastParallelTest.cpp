// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/FastParallel.h>

class FastParallelTest:
  public testing::Test
{};

TEST_F(FastParallelTest, VoidReturn) {
  autowiring::FastParallel p(5);

  std::vector<int> entries(6);
  p.Parallelize([&entries](int i) {
    entries[i] = i;
  }, 0, 6);

  for (int i = 0; i < 6; i++)
    ASSERT_EQ(i, entries[i]);
}

TEST_F(FastParallelTest, ReturnAll) {
  autowiring::FastParallel p(5);

  int begin = 4, end = 10;
  auto entries = p.Parallelize([](int i) {
    return i;
  }, begin, end);

  for (int i = 0; i < end - begin; i++)
    ASSERT_EQ(begin+i, entries[i]);
}

TEST_F(FastParallelTest, IteratorVersion) {
  autowiring::FastParallel p(5);

  auto val = std::make_shared<std::atomic<size_t>>(0);
  std::vector<float> seq = {0.5, 1.5, 2.5, 3.5, 4,5, 5.5};;
  p.Parallelize([val](float f) {
    (*val) = (*val) + f;
  }, seq.begin(), seq.end());

  float sum;
  std::for_each(seq.begin(), seq.end(), [&sum] (float f) {
    sum += f;
  });

  ASSERT_FLOAT_EQ(sum, *val);
}

TEST_F(FastParallelTest, IteratorVersionWithReturn) {
  autowiring::FastParallel p(5);

  std::vector<float> seq = {0.5, 1.5, 2.5, 3.5, 4,5, 5.5};
  auto entries = p.Parallelize([](float f) {
    return f;
  }, seq.begin(), seq.end());

  for (int i = 0; i < seq.size(); i++) {
    ASSERT_EQ(seq[i], entries[i]);
  }
}
