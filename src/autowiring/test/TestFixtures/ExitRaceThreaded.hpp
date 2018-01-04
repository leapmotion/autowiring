// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once
#include <autowiring/autowiring.h>
#include <autowiring/CoreThread.h>

/// <summary>
/// An exit-race signal
/// </summary>
class ExitRaceSignal
{
public:
  ExitRaceSignal(void):
    m_signalled(false)
  {}

  bool m_signalled;
  std::mutex m_lock;
  std::condition_variable m_condVar;

  void Signal(void) {
    std::lock_guard<std::mutex> lk(m_lock);
    m_signalled = true;
    m_condVar.notify_all();
  }

  void Wait(void) {
    std::unique_lock<std::mutex> lk(m_lock);
    m_condVar.wait(lk, [this] {return this->m_signalled;});
  }
};

/// <summary>
/// A class designed to block until a single race marker is set, at which point, it will quit
/// </summary>
/// <remarks>
/// Exit race signals are used instead of a deferred event because it allows many threads to
/// be given the signal to quit at approximately the same time, with a single instruction,
/// because they are all waiting on the same condition to be signalled.
/// </remarks>
class ExitRaceThreaded:
  public CoreThread
{
public:
  Autowired<ExitRaceSignal> m_exitRace;

  void Run(void) override {
    m_exitRace->Wait();
  }
};
