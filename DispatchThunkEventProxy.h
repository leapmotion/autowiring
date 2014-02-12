#pragma once
#include "DispatchThunk.h"
#include FUNCTIONAL_HEADER

class DispatchQueue;
class EventReceiver;

class DispatchThunkEventProxy:
  public DispatchThunkBase
{
public:
  typedef std::function<void (EventReceiver&)> t_eventProxy;

  DispatchThunkEventProxy(DispatchQueue& dq, t_eventProxy&& eventProxy):
    DispatchThunkBase(true),
    dq(dq),
    eventProxy(eventProxy)
  {}

  DispatchQueue& dq;
  t_eventProxy eventProxy;

  void operator()() override;
};
