// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include <math.h>
#include <assert.h>
#include <cctype>
#ifndef NOMINMAX
  #define NOMINMAX
#endif

// Defined when Autowiring is being built, as opposed to when it is being linked
#define AUTOWIRING_IS_BEING_BUILT

#ifndef _MSC_VER
  #include <stdlib.h>
#else
  #define _WINSOCK_DEPRECATED_NO_WARNINGS
  #define _WEBSOCKETPP_CPP11_CHRONO_
  #define _WEBSOCKETPP_LOCALTIME_S_
#endif

// C++11 glue logic, for platforms that have incomplete C++11 support
#include <autowiring/C++11/cpp11.h>

#define ARRAYCOUNT(x) sizeof(ArraySize(x))
