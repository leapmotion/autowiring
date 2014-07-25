// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

// Only include these headers in cases where a pch can be generated
// Currently this is only supported on MSVC
#ifdef _MSC_VER
  #include <thread>
  #define NOMINMAX
#endif
	
#ifndef _MSC_VER
  #include <stdlib.h>
#endif

// C++11 glue logic, for platforms that have incomplete C++11 support
#include "C++11/cpp11.h"
