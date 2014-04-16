#pragma once
#include "EventReceiver.h"
#include <list>

class Object;
class CoreContext;
class BoltBase;

class AutowiringEvents:
  public virtual EventReceiver
{
public:
  virtual void NewContext(const CoreContext&)=0;
  virtual void ExpiredContext(const CoreContext&)=0;
  virtual void NewObject(const CoreContext&, const Object&)=0;
  virtual void EventFired(const CoreContext&, const std::type_info&)=0;
};


