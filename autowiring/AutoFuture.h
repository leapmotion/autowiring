// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "CoreThread.h"
#include "fast_pointer_cast.h"
#include <vector>

/// <summary>
/// Represents a collection of CoreThread entities that can be waited upon
/// </summary>
class AutoFuture
{
public:
  AutoFuture(void);
  ~AutoFuture(void);

private:
  std::vector<std::shared_ptr<CoreThread>> threads;

public:
  /// <summary>
  /// Adds a new entity to be tracked
  /// </summary>
  template<class T>
  void operator+=(const std::shared_ptr<T>& rhs) {
    auto pThread = autowiring::fast_pointer_cast<CoreThread>(rhs);
    if(pThread)
      threads.push_back(pThread);
  }

  /// <summary>
  /// Waits for all of the threads bound in this future
  /// </summary>
  void Wait(void) const {
    for(size_t i = threads.size(); i--;)
      threads[i]->Wait();
  }

  /// <summary>
  /// Waits for all of the threads bound in this future, up to some time limit
  /// </summary>
  template<class DurationType>
  bool WaitFor(DurationType duration) const {
    for(size_t i = threads.size(); i--;)
      if(!threads[i]->WaitFor(duration))
        return false;
    return true;
  }
};
