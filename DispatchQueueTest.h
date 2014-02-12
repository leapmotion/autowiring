#pragma once
#include "EnclosedContextTestBase.h"

class DispatchQueue;

class DispatchQueueTest:
  public EnclosedContextTestBase,
  public DispatchQueue
{
public:
  bool CanAccept(void) const override {return true;}
  bool DelayUntilCanAccept(void) override {return true;}
};
