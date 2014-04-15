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
  //virtual void NewContextMember(const ContextMember&)=0;
  //virtual void NewCoreRunnable(const CoreRunnable&)=0;
  //virtual void NewEventReceiver(const CoreContext&, const EventReceiver&)=0;

  /// <summary>
  /// Fired just before a new bolt is inserted in the specified context
  /// </summary>
  /// <param name="owningContext">The context which now owns the bolt</param>
  /// <param name="bolt">The bolt which is about to be added</param>
  //virtual void NewBolt(const CoreContext& owningContext,const std::type_index& sigil, const BoltBase& bolt) = 0;

  //virtual void NewExceptionFilter(const CoreContext&, const ExceptionFilter&)=0;
  virtual void EventFired(const CoreContext&, const std::type_info&)=0;
};


