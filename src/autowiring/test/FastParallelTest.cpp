// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/FastParallel.h>

class FastParallelTest:
  public testing::Test
{};

TEST_F(FastParallelTest, VoidReturn) {
  autowiring::FastParallel p(5);

  std::vector<int> entries;
  int begin = 4, end = 10;
  p.Parallelize([&entries](int i) {
    entries.push_back(i);
  }, begin, end);

  for (int i = 0; i < end - begin; i++)
    ASSERT_EQ(begin+i, entries[i]);
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

  std::vector<float> seq = {0.5, 1.5, 2.5, 3.5, 4,5, 5.5};
  std::vector<float> entries;
  p.Parallelize([&entries](float f) {
    entries.push_back(f);
  }, seq.begin(), seq.end());

  for (int i = 0; i < seq.size(); i++) {
    ASSERT_EQ(seq[i], entries[i]);
  }
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
