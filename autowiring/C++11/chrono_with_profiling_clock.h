// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
//This solution taken from http://stackoverflow.com/questions/8386128/how-to-get-the-precision-of-high-resolution-clock
//Hopefully it will be able to be depricated when VS2015 hits.
#include <chrono>
#if defined(_MSC_VER) && _MSC_VER < 1900 //high_resolution_clock is broken on all MSVC versions below 2015.

  #include <Windows.h>
  namespace {
    const long long g_Frequency = []() -> long long
    {
      LARGE_INTEGER frequency;
      QueryPerformanceFrequency(&frequency);
      return frequency.QuadPart;
    }();
  }

  namespace std {
    namespace chrono{
      struct profiling_clock
      {
        typedef long long                           rep;
        typedef nano                                period;
        typedef duration<rep, period>               duration;
        typedef time_point<profiling_clock>   time_point;
        static const bool is_steady = true;

        static time_point now() {
          LARGE_INTEGER count;
          QueryPerformanceCounter(&count);
          return time_point(duration(count.QuadPart * static_cast<rep>(period::den) / g_Frequency));
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
