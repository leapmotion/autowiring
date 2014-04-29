#pragma once

#include "ContextMember.h"
#include "DispatchQueue.h"
#include "CoreRunnable.h"
#include FUTURE_HEADER

class Object;

class CoreJob:
  public ContextMember,
  public DispatchQueue,
  public CoreRunnable
{
public:
  CoreJob(const char* name = nullptr);
  virtual ~CoreJob(){};

private:
  // Hold on to this so CoreContext knows we still exist
  std::shared_ptr<Object> m_outstanding;
  
  // Flag, set to true when it's time to start dispatching
  bool m_running;

  // Flag, set to stop when we should stop running
  bool m_shouldStop;
  
  // The current outstanding async in the thread pool, if one exists:
  std::future<void> m_curEvent;

	// Flag, indicating whether curEvent is in a teardown pathway.  This
	// flag is highly stateful.
	bool m_curEventInTeardown;

  /// <summary>
  /// Invokes DispatchAllEvents and safely nullifies the current event
  /// </summary>
  void DispatchAllAndClearCurrent(void);

protected:
  // DispatchQueue overrides
  void OnPended(boost::unique_lock<boost::mutex>&&) override;

  // Resets the outstanding pointer, calls base abort routine
  void Abort(void);

public:
  // Accessor methods:
  bool ShouldStop(void) const { return m_shouldStop; }
  
  // "CoreRunnable" overrides
  bool Start(std::shared_ptr<Object> outstanding) override;
  void Stop(bool graceful) override;
  bool IsRunning(void) const override { return m_running; }
  void Wait(void) override;
};
