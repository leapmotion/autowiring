// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/FastParallel.h>

class FastParallelTest:
  public testing::Test
{};

TEST_F(FastParallelTest, VoidReturn) {
  autowiring::FastParallel p(5);

  std::vector<int64_t> entries(6);
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
    ASSERT_FLOAT_EQ(seq[i], entries[i]);
  }
}

TEST_F(FastParallelTest, MultipleSequences) {
  autowiring::FastParallel p(5);

  std::vector<float> seq1 = {0.5, 1.5, 2.5, 3.5, 4,5, 5.5};
  std::vector<float> entries(6);
  p.Parallelize([&entries](int i, float f) {
    entries[i] = f;
  }, 0, 6, seq1.begin(), seq1.end());
  for (int i = 0; i < 6; i++)
    ASSERT_FLOAT_EQ(seq1[i], entries[i]);

  p.Resize(6);
  std::vector<float> seq2 = {0, 1, 2, 3, 4, 5};
  auto entries = p.Parallelize([](float f1, float f2) {
    return f1 + f2;
  }, seq1.begin(), seq1.end(), seq2.begin(), seq2.end());

  for (int i = 0; i < seq1.size(); i++) {
    ASSERT_FLOAT_EQ(seq1[i] + seq2[i], entries[i]);
  }
}
