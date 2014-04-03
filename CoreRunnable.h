// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once
#include SHARED_PTR_HEADER

class Object;

class CoreRunnable {
public:
  virtual bool Start(std::shared_ptr<Object> outstanding) = 0;
  virtual void Stop(bool graceful) = 0;
  virtual bool IsRunning(void) const = 0;
  virtual void Wait(void) {}
};
