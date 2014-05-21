#pragma once
#include "EventReceiver.h"

class Object;
class CoreContext;

class AutowiringEvents:
  public virtual EventReceiver
{
public:
  virtual void NewContext(CoreContext&)=0;
  virtual void ExpiredContext(CoreContext&)=0;
  virtual void NewObject(CoreContext&, const Object&)=0;
  virtual void EventFired(CoreContext&, const std::type_info&)=0;
};


