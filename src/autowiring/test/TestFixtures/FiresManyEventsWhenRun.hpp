// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "SimpleReceiver.hpp"
#include <autowiring/CoreThread.h>

class FiresManyEventsWhenRun:
  public CoreThread
{
public:
  FiresManyEventsWhenRun(void) :
    totalXmit(0)
  {
  }

  volatile int totalXmit;

  AutoFired<CallableInterface> m_ci;

  void Run(void) override {
    while(!ShouldStop() && totalXmit < 0x7FFFF000)
      // Jam for awhile in an asynchronous way:
    while(++totalXmit % 100)
      m_ci(&CallableInterface::ZeroArgs)();
  }
};
