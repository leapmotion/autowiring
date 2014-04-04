#pragma once
#include "EventReceiver.h"
#include <list>

class CoreContext;
class CoreThread;
class ContextMember;
class EventReceiver;
class BoltBase;
class ExceptionFilter;

class AutowiringEvents:
  public virtual EventReceiver
{
public:
  virtual void NewContext(const CoreContext&)=0;
  virtual void ExpiredContext(const CoreContext&)=0;
  virtual void NewContextMember(const ContextMember&)=0;
  virtual void NewCoreThread(const CoreThread&)=0;
  virtual void NewEventReceiver(const CoreContext&, const EventReceiver&)=0;
  virtual void NewBolt(const CoreContext&, const std::type_index&, const BoltBase&)=0;
  virtual void NewExceptionFilter(const CoreContext&, const ExceptionFilter&)=0;
};