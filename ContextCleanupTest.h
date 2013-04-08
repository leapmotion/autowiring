#pragma once
#include <gtest/gtest.h>

class ContextCleanupTest:
	public testing::Test
{
public:
  ContextCleanupTest(void) {}
  ~ContextCleanupTest(void) {}
};

