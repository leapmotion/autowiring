#pragma once
#include "ContextMember.h"
#include "DispatchQueue.h"
#include "CoreRunnable.h"
#include <future>

class Object;

class CoreJob:
  public ContextMember,
  public DispatchQueue,
  public virtual CoreRunnable
{
private:
  //Hold on to this so CoreContext knows we still exist
  std::shared_ptr<Object> m_outstanding;
  
  // CoreTheads have been started, so start dispatching
  bool m_running;
  
  boost::mutex m_jobLock;
  boost::condition_variable m_jobUpdate;
  
  std::future<void> m_prevEvent;
  
protected:
  // DispatchQueue overrides
  void FireEvent(DispatchThunkBase*) override;
  void OnPended(boost::unique_lock<boost::mutex>&&) override;

public:
  CoreJob(const char* name = nullptr);
  virtual ~CoreJob(){};
  
  // "CoreRunnable" overrides
  bool Start(std::shared_ptr<Object> outstanding) override;
  void Stop(bool graceful=false) override;
  bool IsRunning(void) const override;
  void Wait(void) override;
};
