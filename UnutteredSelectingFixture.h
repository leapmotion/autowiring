#pragma once
#include "ContextMember.h"
#include "MicroBolt.h"

struct SelfSelect;

class UnutteredSelectingFixture:
  public Boltable<SelfSelect>,
  public ContextMember
{
public:
  UnutteredSelectingFixture(void);
  ~UnutteredSelectingFixture(void);
};

