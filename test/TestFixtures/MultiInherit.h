// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "SimpleObject.h"

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
