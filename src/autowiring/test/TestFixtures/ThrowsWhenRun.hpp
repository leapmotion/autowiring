#pragma once
#include "ThrowingListener.hpp"
#include <autowiring/CoreThread.h>

template<class Ex>
class ThrowsWhenRun:
  public CoreThread
{
public:
  // This convoluted syntax is required to evade warnings on Mac
  decltype(throw_rethrowable Ex(100)) MakeException() {
    return throw_rethrowable Ex(100);
  }

  void Run(void) override {
    MakeException();
  }
};
