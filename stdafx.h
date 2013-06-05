// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _STDAFX_H
#define _STDAFX_H

#include <math.h>
#include "ocuConfig.h"

#if defined(_DEBUG) && 0
  #define ENABLE_NETWORK_MONITOR 1
#else
  #define ENABLE_NETWORK_MONITOR 0
#endif

#ifdef _MSC_VER
  #define noexcept throw()
#endif

#endif
