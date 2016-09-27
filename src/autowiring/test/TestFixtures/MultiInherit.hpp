// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include "SimpleObject.hpp"

class DummyBase {
public:
  DummyBase(void):
    x(50),
    y(60)
  {}

  int x;
  int y;
};

class MultiInherit:
  public DummyBase,
  public ContextMember
{
public:
  MultiInherit(void):
    zero(0),
    one(1)
  {}

  int zero;
  int one;
};
