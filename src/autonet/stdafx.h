// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include <math.h>
#include <assert.h>
#define NOMINMAX

#ifndef _MSC_VER
  #include <stdlib.h>
#endif

// C++11 glue logic, for platforms that have incomplete C++11 support
#include "C++11/cpp11.h"

// Very unusual syntax -- function taking an array of fixed size, and returning
// a character array of that same size
template<class T, int n>
const char (&ArraySize(const T (&vals)[n]))[n];

#define ARRAYCOUNT(x) sizeof(ArraySize(x))
