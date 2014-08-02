// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutowiringEnclosure.h"
#include <gtest/gtest-all.cc>

using namespace std;

int main(int argc, char* argv[])
{
  auto& listeners = testing::UnitTest::GetInstance()->listeners();
  listeners.Append(new AutowiringEnclosure);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
