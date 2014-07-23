// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _STDAFX_H
#define _STDAFX_H

#include <math.h>
#include <assert.h>
#include "C++11/cpp11.h"

#ifndef _MSC_VER
  #include <stdlib.h>
#endif

// Very unusual syntax -- function taking an array of fixed size, and returning
// a character array of that same size
template<class T, int n>
const char (&ArraySize(const T (&vals)[n]))[n];

#define ARRAYCOUNT(x) sizeof(ArraySize(x))

#endif
