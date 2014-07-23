// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "C++11/cpp11.h"
#include "src/Autowired.h"
#include <gtest/gtest.h>
#include <thread>
#include <mutex>

// Very unusual syntax -- function taking an array of fixed size, and returning
// a character array of that same size
template<class T, int n>
const char(&ArraySize(const T(&vals)[n]))[n];

#define ARRAYCOUNT(x) sizeof(ArraySize(x))
