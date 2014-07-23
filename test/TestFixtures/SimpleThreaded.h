// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "CoreThread.h"

template<class T>
class SharedPtrReceiver:
  public virtual EventReceiver
{
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
