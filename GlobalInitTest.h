#pragma once
#include <gtest/gtest.h>

class GlobalInitTest:
	public testing::Test
{
public:
  GlobalInitTest(void) {}
  ~GlobalInitTest(void) {}

  void TearDown(void);
};

