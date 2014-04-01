#pragma once
#include "ContextMember.h"
#include "DispatchQueue.h"

class AutoJob:
  public ContextMember,
  public DispatchQueue
{
public:
  AutoJob(const char* name = nullptr);

private:
  boost::mutex m_lock;
  boost::condition_variable m_stateCondition;
  
  // Acceptor flag:
  bool m_canAccept;

public:
  // EventDispatcher overrides:
  bool DelayUntilCanAccept(void) override;
  bool CanAccept(void) const override;
  
  // "CoreThread" stuff
  bool ShouldStop(void) const;
  void AcceptDispatchDelivery(void);
  void RejectDispatchDelivery(void);
};
