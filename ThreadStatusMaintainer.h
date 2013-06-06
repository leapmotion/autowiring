// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _THREAD_STATUS_MAINTAINER_H
#define _THREAD_STATUS_MAINTAINER_H
#include SHARED_PTR_HEADER

class CoreContext;
class CoreThread;
class OutstandingCountTracker;

class ThreadStatusMaintainer
{
public:
  ThreadStatusMaintainer(CoreThread* pThread, const std::shared_ptr<CoreContext>& context);

private:
  CoreThread* pThread;
  std::shared_ptr<CoreContext> context;
  std::shared_ptr<OutstandingCountTracker> outstanding;

public:
  void operator()();
};
#endif