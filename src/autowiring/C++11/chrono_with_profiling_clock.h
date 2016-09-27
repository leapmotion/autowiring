// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once

#if STL11_ALLOWED
  #include <chrono>
#else
  #include <autowiring/C++11/boost_chrono.h>
#endif

// This solution taken from http://stackoverflow.com/questions/8386128/how-to-get-the-precision-of-high-resolution-clock
// Hopefully it will be able to be depricated when VS2015 hits.

// high_resolution_clock is broken on all MSVC versions below 2015.
#if defined(_MSC_VER) && _MSC_VER < 1900
union _LARGE_INTEGER;
typedef _LARGE_INTEGER LARGE_INTEGER;

extern "C" {
  __declspec(dllimport)
  int __stdcall QueryPerformanceCounter(LARGE_INTEGER * lpPerformanceCount);

  __declspec(dllimport)
  int __stdcall QueryPerformanceFrequency(LARGE_INTEGER * lpFrequency);
}

namespace {
  const double g_nanosecs_per_tic = [] {
    int64_t frequency;
    QueryPerformanceFrequency(&reinterpret_cast<LARGE_INTEGER&>(frequency));
    return 1e9 / static_cast<double>(frequency);
  }();
}

namespace std {
  namespace chrono {
    struct profiling_clock
    {
      typedef long long rep;
      typedef nano period;
      typedef duration<rep, period> duration;
      typedef time_point<profiling_clock> time_point;
      static const bool is_steady = true;

      static time_point now(void) {
        int64_t count;
        QueryPerformanceCounter(&reinterpret_cast<LARGE_INTEGER&>(count));

        // A cast to double is necessary, here, due to the fact that a potentially very large number
        // is being multiplied by a very small number.
        return time_point{
          duration{
            static_cast<int64_t>(static_cast<double>(count) * g_nanosecs_per_tic)
          }
        };
      }
    };
  }
}
#else
namespace std {
  namespace chrono {
    typedef high_resolution_clock profiling_clock;
  }
}
#endif
