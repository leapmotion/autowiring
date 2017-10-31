// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include <autowiring/C++11/cpp11.h>
#include CHRONO_HEADER
#include <autowiring/basic_timer.h>
#include <cstdint>

//Common chrono types
#define AW_DEFINE_CHRONO_ALIAS(type,suffix) \
  typedef std::chrono::duration<type> seconds_##suffix; \
  typedef std::chrono::duration<type, std::milli> milliseconds_##suffix; \
  typedef std::chrono::duration<type, std::micro> microseconds_##suffix; \
  typedef std::chrono::duration<type, std::nano> nanoseconds_##suffix; \
  typedef std::chrono::duration<type, std::pico> picoseconds_##suffix; \
  typedef std::chrono::duration<type, std::femto> femtoseconds_##suffix;

AW_DEFINE_CHRONO_ALIAS(double, d)
AW_DEFINE_CHRONO_ALIAS(float, f)
AW_DEFINE_CHRONO_ALIAS(int32_t, i32)
AW_DEFINE_CHRONO_ALIAS(int64_t, i64)
AW_DEFINE_CHRONO_ALIAS(uint32_t, u32)
AW_DEFINE_CHRONO_ALIAS(uint64_t, u64)

typedef basic_timer<std::chrono::profiling_clock> profiling_timer;
typedef basic_timer<std::chrono::profiling_clock, seconds_d> profiling_timer_seconds;
typedef basic_timer<std::chrono::profiling_clock, milliseconds_d> profiling_timer_milliseconds;
typedef basic_timer<std::chrono::profiling_clock, microseconds_d> profiling_timer_microseconds;
typedef basic_timer<std::chrono::profiling_clock, nanoseconds_d> profiling_timer_nanoseconds;
typedef basic_timer<std::chrono::profiling_clock, picoseconds_d> profiling_timer_picoseconds;
typedef basic_timer<std::chrono::profiling_clock, femtoseconds_d> profiling_timer_femtoseconds;
