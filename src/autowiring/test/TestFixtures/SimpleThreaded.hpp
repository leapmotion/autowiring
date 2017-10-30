// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once

#include <autowiring/CoreThread.h>

/// <summary>
/// A simple class which will delay block until it is signalled to stop.
/// </summary>
template<class T>
class SimpleThreadedT:
  public CoreThread
{
public:
  SimpleThreadedT(void) {}

  void OnEvent(std::shared_ptr<T> obj) {
    *this += [obj]{};
  }
};

typedef SimpleThreadedT<int> SimpleThreaded;
