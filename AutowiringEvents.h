#pragma once
#include "EventReceiver.h"

class CoreContext;
class ContextMember;

class AutowiringEvents:
  public virtual EventReceiver
{
public:
  virtual void NewContext(const CoreContext&)=0;
  virtual void ExpiredContext(const CoreContext&)=0;
  virtual void NewContextMember(const ContextMember&)=0;
};