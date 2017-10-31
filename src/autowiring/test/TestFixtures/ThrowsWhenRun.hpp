// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include "ThrowingListener.hpp"
#include <autowiring/CoreThread.h>

template<class Ex>
class ThrowsWhenRun:
  public CoreThread
{
public:
  // This convoluted syntax is required to evade warnings on Mac
  decltype(throw Ex(100)) MakeException() {
    return throw Ex(100);
  }

  void Run(void) override {
    MakeException();
  }
};
