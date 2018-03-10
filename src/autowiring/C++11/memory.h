// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once

#include <memory>

// MSVC and C++14 already implements make_unique
#if !defined(_MSC_VER) && (__cplusplus < 201402L)
  #include "make_unique.h"
#endif
