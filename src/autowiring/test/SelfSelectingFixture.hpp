// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include <autowiring/ContextMember.h>
#include <autowiring/MicroBolt.h>

struct SelfSelect;

class SelfSelectingFixture:
  public Boltable<SelfSelect>,
  public ContextMember
{
public:
  SelfSelectingFixture(void);

private:
  int magic;

public:
  /// <returns>
  /// True if the internally-held magic value is initialized to the correct value
  /// </returns>
  bool IsMagicCorrect(void) const;
};
