#pragma once
#include <gtest/gtest.h>

class GlobalInitTest:
  public testing::Test
{
public:
  GlobalInitTest(void) {}
  ~GlobalInitTest(void) {}

  void SetUp(void);
  void TearDown(void);
};

