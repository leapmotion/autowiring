#pragma once
#include "ContextMember.h"
#include "DispatchQueue.h"

class AutoJob:
  public ContextMember,
  public DispatchQueue
{
public:
  AutoJob(const char* name = nullptr);

private:

public:
  // EventDispatcher overrides:
  bool DelayUntilCanAccept(void) override;
  bool CanAccept(void) const override;
};