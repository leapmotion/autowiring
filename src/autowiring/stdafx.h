// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once

// Internal build flag for namespace discrimination
#define AUTOWIRING_IS_BEING_BUILT

// Only include these headers in cases where a pch can be generated
// Currently this is only supported on MSVC
#ifdef _MSC_VER
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif

  #include <atomic>
  #include <chrono>
  #include <exception>
  #include <iosfwd>
  #include <memory>
  #include <mutex>
  #include <queue>
  #include <set>
  #include <string>
  #include <thread>
  #include <type_traits>
  #include <unordered_map>
  #include <vector>
#endif

#ifndef _MSC_VER
  #include <stdlib.h>
#endif

// Preconfiguration:
#include "AutowiringConfig.h"

// C++11 glue logic, for platforms that have incomplete C++11 support
#include "C++11/cpp11.h"
