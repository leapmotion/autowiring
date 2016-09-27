// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
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
