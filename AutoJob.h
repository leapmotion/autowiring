#pragma once
#include "ContextMember.h"
#include "DispatchQueue.h"
#include "CoreRunnable.h"

class Object;

class AutoJob:
  public ContextMember,
  public DispatchQueue,
  public virtual CoreRunnable
{
public:
  AutoJob(const char* name = nullptr);

private:
  //Hold on to this so CoreContext knows we still exist
  std::shared_ptr<Object> m_outstanding;
  
  // Acceptor flag:
  bool m_canAccept;

protected:
  void FireEvent(DispatchThunkBase&) override;

public:
  // DispatchQueue overrides:
  bool DelayUntilCanAccept(void) override;
  bool CanAccept(void) const override;
  
  // "CoreThread" stuff
  bool Start(std::shared_ptr<Object> outstanding) override;
  void Stop(bool graceful=false) override;
};
