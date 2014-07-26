#pragma once
#include <autowiring/EventReceiver.h>

class ThrowingListener:
  public virtual EventReceiver
{
public:
  ThrowingListener(void) {}
  virtual void DoThrow(void) = 0;
};
