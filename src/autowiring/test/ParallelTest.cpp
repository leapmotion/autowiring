// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "gtest-all-guard.hpp"
#include <autowiring/Parallel.h>
#include <algorithm>
#include <thread>
#include <random>

class ParallelTest:
  public testing::Test
{};

TEST_F(ParallelTest, Basic) {
  AutoCurrentContext()->Initiate();
  autowiring::parallel p;

  std::mt19937_64 mt(time(nullptr));
  std::uniform_int_distribution<int> dist(0, 500);

  for (int i : {0,4,2,5,1,3}) {
    int sleepTime = dist(mt);
    p += [i, sleepTime]() {
      std::this_thread::sleep_for(sleepTime*std::chrono::milliseconds(1));
      return i;
    };
  }

  std::vector<int> result;
  auto it = p.begin<int>();
  result.push_back(*it++);
  while (it != p.end<int>()) {
    result.push_back(*it);
    ++it;
  }

  ASSERT_EQ(result.size(), 6) << "Didn't receive all value";

  std::sort(result.begin(), result.end());
  for (int i = 0; i < static_cast<int>(result.size()); ++i) {
    ASSERT_EQ(i, result[i]) << "Didn't receive correct values";
  }
}

TEST_F(ParallelTest, All) {
  AutoCurrentContext()->Initiate();
  autowiring::parallel p;

  for (size_t i = 0; i < 10; i++)
    p += [i] { return i; };

  std::vector<size_t> entries;
  for(size_t cur : p.all<size_t>())
    entries.push_back(cur);
  std::sort(entries.begin(), entries.end());
  for (size_t i = 1; i < entries.size(); i++)
    ASSERT_EQ(entries[i - 1], entries[i] - 1) << "Entry did not complete as expected";
}
