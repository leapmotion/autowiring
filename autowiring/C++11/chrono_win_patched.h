#pragma once

#define high_resolution_clock broken_native_high_resolution_clock
#include <chrono>
#undef high_resolution_clock

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
    struct high_resolution_clock
    {
      typedef long long                           rep;
      typedef nano                                period;
      typedef duration<rep, period>               duration;
      typedef time_point<high_resolution_clock>   time_point;
      static const bool is_steady = true;

      static time_point now() {
        LARGE_INTEGER count;
        QueryPerformanceCounter(&count);
        return time_point(duration(count.QuadPart * static_cast<rep>(period::den) / g_Frequency));
      }
    };
  }
}