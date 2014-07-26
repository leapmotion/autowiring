#pragma once
#include "custom_exception.hpp"
#include "ThrowingListener.hpp"

template<class Ex = custom_exception, int i = 200>
class ThrowsWhenFired :
  public ThrowingListener
{
public:
  ThrowsWhenFired(void) :
    hit(false)
  {}

  bool hit;

  void DoThrow(void) override {
    hit = true;
    throw_rethrowable Ex(i);
  }
};
