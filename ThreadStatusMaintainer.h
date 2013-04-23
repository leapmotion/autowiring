#pragma once

class CoreContext;
class CoreThread;
class OutstandingCountTracker;

class ThreadStatusMaintainer
{
public:
  ThreadStatusMaintainer(CoreThread* pThread, const cpp11::shared_ptr<CoreContext>& context);
  ~ThreadStatusMaintainer(void);

private:
  CoreThread* pThread;
  cpp11::shared_ptr<CoreContext> context;
  cpp11::shared_ptr<OutstandingCountTracker> outstanding;

public:
  void operator()();
};

