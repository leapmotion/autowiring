// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _SIMPLE_THREADED_H
#define _SIMPLE_THREADED_H
#include "Autowiring/CoreThread.h"

/// <summary>
/// A simple class which will delay block until it is signalled to stop.
/// </summary>
class SimpleThreaded:
  public CoreThread
{
public:
  SimpleThreaded(void) {
    Ready();
  }
};

#endif
