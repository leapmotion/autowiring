#pragma once
#include "ContextMember.h"
#include "DispatchQueue.h"
#include "CoreRunnable.h"
#include <future>

class Object;

class AutoJob:
  public ContextMember,
  public DispatchQueue,
  public virtual CoreRunnable
{
private:
  //Hold on to this so CoreContext knows we still exist
  std::shared_ptr<Object> m_outstanding;
  
  // Acceptor flag:
  bool m_canAccept;
  
  boost::mutex m_jobLock;
  boost::condition_variable m_jobUpdate;
  
  std::future<void> m_prevEvent;
  
protected:
  void FireEvent(DispatchThunkBase*) override;
  

public:
  AutoJob(const char* name = nullptr);
  virtual ~AutoJob(){};
  
  // DispatchQueue overrides:
  bool DelayUntilCanAccept(void) override;
  bool CanAccept(void) const override;
  
  // "CoreRunnable" overrides
  bool Start(std::shared_ptr<Object> outstanding) override;
  void Stop(bool graceful=false) override;
  bool IsRunning(void) const override;
  void Wait(void) override;
};
