#pragma once

class CoreContext;
class CoreThread;
class OutstandingCountTracker;

class ThreadStatusMaintainer
{
public:
  ThreadStatusMaintainer(CoreThread* pThread, const std::shared_ptr<CoreContext>& context);
  ~ThreadStatusMaintainer(void);

private:
  CoreThread* pThread;
  std::shared_ptr<CoreContext> context;
  std::shared_ptr<OutstandingCountTracker> outstanding;

public:
  void operator()();
};

