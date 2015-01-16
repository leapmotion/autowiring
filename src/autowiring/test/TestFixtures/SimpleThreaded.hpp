// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <autowiring/CoreThread.h>

template<class T>
class SharedPtrReceiver {
public:
  virtual void OnEvent(std::shared_ptr<T> obj) = 0;
};

/// <summary>
/// A simple class which will delay block until it is signalled to stop.
/// </summary>
template<class T>
class SimpleThreadedT:
  public CoreThread,
  public SharedPtrReceiver<T>
{
public:
  SimpleThreadedT(void) {}

  void OnEvent(std::shared_ptr<T> obj) override {
    *this += [obj]{};
  }
};

typedef SimpleThreadedT<int> SimpleThreaded;
