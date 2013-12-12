#pragma once
#include "ContextMember.h"
#include "MicroBolt.h"

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