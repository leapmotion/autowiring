// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autotesting/gtest-all-guard.hpp>
#include <autowiring/autowiring.h>
#include <autowiring/AutowiringDebug.h>
#include <autowiring/CoreThread.h>

// Version number should have at least two dots, three digits, and a null terminator
static_assert(sizeof(AUTOWIRING_VERSION_STR) >= 2 + 3 + 1, "Version number string appears to be incorrect");

// Version number field itself must be at least 0x10003, which corresponds to v1.0.3
static_assert(0x10003 <= AUTOWIRING_VERSION, "Version number appears to have gone backwards");

// Other version number fields should be at least one character each
static_assert(sizeof(AUTOWIRING_VERSION_MAJOR) >= 2, "Major version number component appears to be incorrect");
static_assert(sizeof(AUTOWIRING_VERSION_MINOR) >= 2, "Minor version number component appears to be incorrect");
static_assert(sizeof(AUTOWIRING_VERSION_PATCH) >= 2, "Patch version number component appears to be incorrect");

int main(int argc, const char* argv []) {
  autowiring::dbg::DebugInit();
  return autotesting_main(argc, argv);
}

class AutowiringTest:
  public testing::Test
{};
