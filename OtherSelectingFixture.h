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

