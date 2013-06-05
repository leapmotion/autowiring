// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _STDAFX_H
#define _STDAFX_H

#include <math.h>
#include "ocuConfig.h"

// We assume platform rethrow exists if it isn't specified explicitly
#ifndef PLATFORM_RETHROW_EXISTS
  #define PLATFORM_RETHROW_EXISTS 1
#endif

#if defined(_DEBUG) && 0
  #define ENABLE_NETWORK_MONITOR 1
#else
  #define ENABLE_NETWORK_MONITOR 0
#endif

#endif
