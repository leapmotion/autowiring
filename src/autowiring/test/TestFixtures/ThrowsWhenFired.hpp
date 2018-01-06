// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
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
    throw Ex(i);
  }
};
