// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

// Only include these headers in cases where a pch can be generated
// Currently this is only supported on MSVC
#ifdef _MSC_VER
  #include <thread>
  #define NOMINMAX
#endif

#ifndef _MSC_VER
  #include <stdlib.h>
#endif

// C++11 glue logic, for platforms that have incomplete C++11 support
#include "contrib/C++11/cpp11.h"


template<class T, int n>
const char(&ArraySize(const T(&vals)[n]))[n];
#define ARRAYCOUNT(x) sizeof(ArraySize(x))