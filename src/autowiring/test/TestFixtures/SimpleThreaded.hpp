// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <autowiring/CoreThread.h>

template<class T>
class SharedPtrReceiver {
public:
  virtual Deferred OnEvent(std::shared_ptr<T> obj) = 0;
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

  Deferred OnEvent(std::shared_ptr<T> obj) override {
    return Deferred(this);
  }
};

typedef SimpleThreadedT<int> SimpleThreaded;
