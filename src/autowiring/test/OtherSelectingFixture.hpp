// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include <autowiring/ContextMember.h>
#include <autowiring/MicroBolt.h>

struct SelfSelect;

class OtherSelectingFixture:
  public Boltable<SelfSelect>,
  public ContextMember
{
public:
  OtherSelectingFixture(void);
  ~OtherSelectingFixture(void);
};
