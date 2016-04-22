// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/C++11/cpp11.h>
#include <gtest/gtest.h>
#include <autoboost/filesystem/config.hpp>
#include <autoboost/filesystem/convenience.hpp>
#include <autoboost/filesystem/operations.hpp>
#include <autoboost/filesystem/path.hpp>
#include FILESYSTEM_HEADER

class FileSystemHeaderTest:
  public testing::Test
{};

TEST_F(FileSystemHeaderTest, PathPropertiesTest) {
  std::filesystem::path p = "abc/def.jpg";
  ASSERT_STREQ(L".jpg", p.extension().c_str());
  ASSERT_STREQ(L"def.jpg", p.filename().c_str());
}
