// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once

// Internal build flag for namespace discrimination
#define AUTOWIRING_IS_BEING_BUILT

// Only include these headers in cases where a pch can be generated
// Currently this is only supported on MSVC
#ifdef _MSC_VER
  #include <thread>
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
#endif

#ifndef _MSC_VER
  #include <stdlib.h>
#endif

// C++11 glue logic, for platforms that have incomplete C++11 support
#include <autowiring/C++11/cpp11.h>


template<class T, int n>
const char(&ArraySize(const T(&vals)[n]))[n];
#define ARRAYCOUNT(x) sizeof(ArraySize(x))
