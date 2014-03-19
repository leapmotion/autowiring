#pragma once
#include "EventReceiver.h"
#include SHARED_PTR_HEADER

class CoreContext;

class AutowiringEvents:
  public virtual EventReceiver
{
public:
  virtual void NewContext(std::shared_ptr<CoreContext>)=0;
  virtual void ExpiredContext(std::shared_ptr<CoreContext>)=0;
};