// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <autowiring/C++11/cpp11.h>
#include CHRONO_HEADER
#include "chrono_timer.h"

//Common chrono types
typedef std::chrono::duration<double> seconds_d;
typedef std::chrono::duration<double, std::milli> milliseconds_d;
typedef std::chrono::duration<double, std::micro> microseconds_d;
typedef std::chrono::duration<double, std::nano> nanoseconds_d;

typedef std::chrono::duration<float> seconds_f;
typedef std::chrono::duration<float, std::milli> milliseconds_f;
typedef std::chrono::duration<float, std::micro> microseconds_f;
typedef std::chrono::duration<float, std::nano> nanoseconds_f;

typedef basic_timer<std::chrono::profiling_clock> profiling_timer;
typedef basic_timer<std::chrono::profiling_clock, seconds_d> profiling_timer_seconds;
typedef basic_timer<std::chrono::profiling_clock, milliseconds_d> profiling_timer_milliseconds;
typedef basic_timer<std::chrono::profiling_clock, microseconds_d> profiling_timer_microseconds;
