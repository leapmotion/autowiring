// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <C++11/cpp11.h>
#include <gtest/gtest.h>

#ifdef _MSC_VER
  #include <autowiring/Autowired.h>
  #include <thread>
  #include <mutex>
#endif

// Very unusual syntax -- function taking an array of fixed size, and returning
// a character array of that same size
template<class T, int n>
const char(&ArraySize(const T(&vals)[n]))[n];

#define ARRAYCOUNT(x) sizeof(ArraySize(x))
