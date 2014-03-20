#pragma once
#include "EventReceiver.h"

class CoreContext;

class AutowiringEvents:
  public virtual EventReceiver
{
public:
  virtual void NewContext(CoreContext&)=0;
  virtual void ExpiredContext(CoreContext&)=0;
};