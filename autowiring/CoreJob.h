// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "ContextMember.h"
#include "DispatchQueue.h"
#include "CoreRunnable.h"

class Object;

class CoreJob:
  public ContextMember,
  public DispatchQueue,
  public CoreRunnable
{
public:
  CoreJob(const char* name = nullptr);
  virtual ~CoreJob(void);

private:
  // Flag, set to true when it's time to start dispatching
  bool m_running;

  // The current outstanding async in the thread pool, if one exists:
  void* m_curEvent;

  // Flag, indicating whether curEvent is in a teardown pathway.  This
  // flag is highly stateful.
  bool m_curEventInTeardown;

  /// <summary>
  /// Invokes DispatchAllEvents and safely nullifies the current event
  /// </summary>
  void DispatchAllAndClearCurrent(void);

protected:
  // DispatchQueue overrides
  void OnPended(std::unique_lock<std::mutex>&&) override;

  // Resets the outstanding pointer, calls base abort routine
  void Abort(void);

public:
  // "CoreRunnable" overrides
  bool OnStart(void) override;
  void OnStop(bool graceful) override;
  void DoAdditionalWait(void) override;
};
