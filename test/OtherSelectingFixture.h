// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "ContextMember.h"
#include "MicroBolt.h"

struct SelfSelect;

class OtherSelectingFixture:
  public Boltable<SelfSelect>,
  public ContextMember
{
public:
  OtherSelectingFixture(void);
  ~OtherSelectingFixture(void);
};
