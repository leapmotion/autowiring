// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/observable.h>
#include <set>
#include <unordered_set>

class ObservableTest:
  public testing::Test
{};

TEST_F(ObservableTest, SimpleAssignmentCheck) {
  autowiring::observable<int> ob;

  bool hit = false;
  ob.onChanged += [&hit] { hit = true; };
  ob = 2;
  ASSERT_TRUE(hit) << "OnChanged handler not hit when the observable value was modified";
  ASSERT_EQ(2, *ob) << "operator * gives an incorrect value";
}

TEST_F(ObservableTest, BeforeAndAfter) {
  autowiring::observable<int> ob;
  ob = 8;

  bool hit = false;
  ob.onChanged += [&hit] { hit = true; };

  int obBefore = 0;
  int obAfter = 0;
  ob.onBeforeChanged += [&] (const int& before, const int& after) {
    obBefore = before;
    obAfter = after;
  };

  ob = 9;

  ASSERT_TRUE(hit) << "Change notification not raised";
  ASSERT_EQ(8, obBefore) << "\"Before\" value in onBeforeChanged was not correct";
  ASSERT_EQ(9, obAfter) << "\"AfFter\" value in onBeforeChanged was not correct";
}

TEST_F(ObservableTest, SetOfObservable) {
  std::unordered_set<autowiring::observable<int>> a;
  a.insert(4449);
  a.insert(44410);
  a.insert(44411);
  ASSERT_EQ(1, a.count(4449));
  ASSERT_EQ(1, a.count(44410));
  ASSERT_EQ(1, a.count(44411));

  std::set<autowiring::observable<int>> b;
  b.insert(9);
  b.insert(12);
  b.insert(44);
  ASSERT_EQ(1, b.count(9));
  ASSERT_EQ(1, b.count(12));
  ASSERT_EQ(1, b.count(44));
}
