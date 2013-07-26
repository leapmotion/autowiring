#ifndef _EXIT_RACE_THREADED_H
#define _EXIT_RACE_THREADED_H
#include "Autowiring/CoreThread.h"
#include <boost/thread/condition_variable.hpp>

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
  boost::mutex m_lock;
  boost::condition_variable m_condVar;

  void Signal(void) {
    boost::lock_guard<boost::mutex> lk(m_lock);
    m_signalled = true;
    m_condVar.notify_all();
  }

  void Wait(void) {
    boost::unique_lock<boost::mutex> lk(m_lock);
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
  ExitRaceThreaded(void) {
    Ready();
  }

  Autowired<ExitRaceSignal> m_exitRace;

  void Run(void) override {
    m_exitRace->Wait();
  }
};

#endif