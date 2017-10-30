// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once

class GlobalInitTest:
  public testing::Test
{
public:
  GlobalInitTest(void) {}
  ~GlobalInitTest(void) {}

  void SetUp(void);
  void TearDown(void);
};
