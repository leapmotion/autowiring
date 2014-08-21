// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include <math.h>
#include <assert.h>
#include <cctype>
#ifndef NOMINMAX
  #define NOMINMAX
#endif

#ifndef _MSC_VER
  #include <stdlib.h>
#endif

// C++11 glue logic, for platforms that have incomplete C++11 support
#include "C++11/cpp11.h"

#define ARRAYCOUNT(x) sizeof(ArraySize(x))
