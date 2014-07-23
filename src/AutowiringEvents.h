#pragma once
#include "EventReceiver.h"
#include TYPE_INDEX_HEADER

struct AnySharedPointer;
class CoreContext;

class AutowiringEvents:
  public virtual EventReceiver
{
public:
  virtual void NewContext(CoreContext&)=0;
  virtual void ExpiredContext(CoreContext&)=0;
  virtual void NewObject(CoreContext&, const AnySharedPointer&)=0;
  virtual void EventFired(CoreContext&, const std::type_info&)=0;
};


