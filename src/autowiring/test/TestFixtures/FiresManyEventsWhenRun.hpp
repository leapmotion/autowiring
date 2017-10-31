// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include "SimpleReceiver.hpp"
#include <autowiring/CoreThread.h>
#include <thread>

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
    while(!ShouldStop() && totalXmit < 0x7FFFF000) {
        // Jam for awhile in an asynchronous way:
      while(++totalXmit % 100)
        m_ci(&CallableInterface::ZeroArgs)();
      if (totalXmit % 2000 == 0)
        std::this_thread::yield();
    }
  }
};
