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
    p += [i, sleepTime] {
      std::this_thread::sleep_for(sleepTime*std::chrono::microseconds(1));
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

TEST_F(ParallelTest, VoidReturn) {
  AutoCurrentContext()->Initiate();
  autowiring::parallel p;

  auto val = std::make_shared<std::atomic<size_t>>(0);
  for (size_t i = 0; i < 100; i++)
    p += [val] { (*val)++; };

  size_t i = 0;
  for (auto q = p.begin<void>(); q != p.end<void>(); ++q)
    i++;
  ASSERT_EQ(100UL, i) << "A sufficient number of empty lambdas were not encountered";
}

TEST_F(ParallelTest, VoidReturnAll) {
  AutoCurrentContext()->Initiate();
  autowiring::parallel p;

  auto val = std::make_shared<std::atomic<size_t>>(0);
  for (size_t i = 0; i < 100; i++)
    p += [val] { (*val)++; };

  size_t i = 0;
  for (auto entry : p.all<void>())
    i++;
  ASSERT_EQ(100UL, i) << "A sufficient number of empty lambdas were not encountered";
  ASSERT_EQ(100, *val) << "Not all pended lambda functions were called as expected";
}

TEST_F(ParallelTest, Barrier) {
  AutoCurrentContext()->Initiate();
  autowiring::parallel p;

  std::atomic<size_t> x{ 0 };
  for (size_t i = 0; i < 1000; i++)
    p += [&x] { x++; };

  p.barrier();
  ASSERT_EQ(1000, x) << "Not all parallel watchers were completed on return from join";
}
