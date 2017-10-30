// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "PriorityBoost.h"
#include "Benchmark.h"
#include <autowiring/CoreThread.h>
#include <stdexcept>
#include <thread>

#ifdef _MSC_VER
#include <windows.h>
#endif

template<ThreadPriority priority>
class JustIncrementsANumber:
  public CoreThread
{
public:
  JustIncrementsANumber() :
    val(0)
  {}

  volatile int64_t val;

  // This will be a hotly contested conditional variable
  AutoRequired<std::mutex> contended;

  void Run(void) override {
    ElevatePriority p(*this, priority);

    while(!ShouldStop()) {
      // Obtain the lock and then increment our value:
      std::lock_guard<std::mutex> lk(*contended);
      val++;
    }
  }
};

Benchmark PriorityBoost::CanBoostPriority(void) {
  AutoCurrentContext ctxt;

  // Create two spinners and kick them off at the same time:
  AutoRequired<JustIncrementsANumber<ThreadPriority::BelowNormal>> lower;
  AutoRequired<JustIncrementsANumber<ThreadPriority::Normal>> higher;
  ctxt->Initiate();

#ifdef _MSC_VER
  // We want all of our threads to run on ONE cpu for awhile, and then we want to put it back at exit
  DWORD_PTR originalAffinity, systemAffinity;
  GetProcessAffinityMask(GetCurrentProcess(), &originalAffinity, &systemAffinity);
  SetProcessAffinityMask(GetCurrentProcess(), 1);
  auto onreturn = MakeAtExit([originalAffinity] {
    SetProcessAffinityMask(GetCurrentProcess(), originalAffinity);
  });
#else
  // TODO:  Implement on Unix so that this benchmark is trustworthy
#endif

  // Poke the conditional variable a lot:
  AutoRequired<std::mutex> contended;
  for(size_t i = 100; i--;) {
    // We sleep while holding contention lock to force waiting threads into the sleep queue.  The reason we have to do
    // this is due to the way that mutex is implemented under the hood.  The STL mutex uses a high-frequency variable
    // and attempts to perform a CAS (check-and-set) on this variable.  If it succeeds, the lock is obtained; if it
    // fails, it will put the thread into a non-ready state by calling WaitForSingleObject on Windows or one of the
    // mutex_lock methods on Unix.
    //
    // When a thread can't be run, it's moved from the OS's ready queue to the sleep queue.  The scheduler knows that
    // the thread can be moved back to the ready queue if a particular object is signalled, but in the case of a lock,
    // only one of the threads waiting on the object can actually be moved to the ready queue.  It's at THIS POINT that
    // the operating system consults the thread priority--if only thread can be moved over, then the highest priority
    // thread will wind up in the ready queue every time.
    //
    // Thread priority does _not_ necessarily influence the amount of time the scheduler allocates allocated to a ready
    // thread with respect to other threads of the same process.  This is why we hold the lock for a full millisecond,
    // in order to force the thread over to the sleep queue and ensure that the priority resolution mechanism is
    // directly tested.
    std::lock_guard<std::mutex> lk(*contended);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  // Need to terminate before we try running a comparison.
  ctxt->SignalTerminate();

  return Benchmark {
    {"Low priority CPU time", std::chrono::nanoseconds{lower->val}},
    {"High priority CPU time", std::chrono::nanoseconds{higher->val}},
  };
}
