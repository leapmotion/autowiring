#pragma once
#include "BasicThread.h"
#include "DispatchQueue.h"
#include MEMORY_HEADER

class CoreContext;
class CoreThread;

/// <summary>
/// This is an abstract class that has a single Run method for implementation by a
/// derived class.  The object will remain in the context as long as the thread is
/// running, even if there are no other references to it.
/// </summary>
class CoreThread:
  public BasicThread,
  public DispatchQueue
{
public:
  /// <param name="pName">An optional name for this thread</param>
  CoreThread(const char* pName = nullptr);

  virtual ~CoreThread(void) {}

protected:
  void DEPRECATED(Ready(void) const, "Do not call this method, the concept of thread readiness is now deprecated") {}

  /// <summary>
  /// Overridden here so we can rundown the dispatch queue
  /// </summary>
  virtual void DoRunLoopCleanup(std::shared_ptr<CoreContext>&& ctxt, std::shared_ptr<Object>&& refTracker) override;

public:
  /// <summary>
  /// Blocks until a new dispatch event is added, dispatches that single event, and then returns
  /// </summary>
  void WaitForEvent(void);

  /// <summary>
  /// Timed version of WaitForEvent
  /// </summary>
  /// <returns>
  /// False if the timeout period elapsed before an event could be dispatched, true otherwise
  /// </returns>
  bool WaitForEvent(boost::chrono::milliseconds milliseconds);

  /// <summary>
  /// Wakeup-point version of WaitForEvent
  /// </summary>
  /// <returns>
  /// False if the timeout period elapsed before an event could be dispatched, true otherwise
  /// </returns>
  bool WaitForEvent(boost::chrono::high_resolution_clock::time_point wakeTime);

  /// <summary>
  /// An unsafe variant of WaitForEvent
  /// </summary>
  bool WaitForEventUnsafe(boost::unique_lock<boost::mutex>& lk, boost::chrono::high_resolution_clock::time_point wakeTime);

  /// <summary>
  /// Begins the core thread
  /// </summary>
  /// <param name="context">A shared pointer to the context containing this thread</param>
  /// <remarks>
  /// The default implementation of Run will simply call WaitForEvent in a loop until it's
  /// told to quit.
  /// </remarks>
  virtual void Run() override;

  /// <summary>
  /// Event which may be used to perform custom handling when the thread is told to stop
  /// </summary>
  /// <param name="graceful">Set to true to rundown the dispatch queue before quitting</param>
  /// <remarks>
  /// This method is called when the thread should stop.  When invoked, the value of
  /// CoreThread::ShouldStop is guaranteed to be true.
  ///
  /// Callers are not required to call CoreThread::OnStop.  This method is guaranteed to do
  /// nothing by default.
  /// </remarks>
  virtual void Stop(bool graceful = false) override;
  
  /// <summary>
  /// Returns the percent of time this thread is processing events from DispatchQueue
  /// </summary>
  double GetThreadUtilization();
};
