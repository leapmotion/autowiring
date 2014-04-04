#pragma once
#include "BasicThread.h"
#include "DispatchQueue.h"
#include SHARED_PTR_HEADER

using std::shared_ptr;

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
  // Acceptor flag:
  bool m_canAccept;

protected:
  void DEPRECATED(Ready(void) const, "Do not call this method, the concept of thread readiness is now deprecated") {}
  
  /// <summary>
  /// Routine that sets up the necessary extranea before a call to Run
  /// </summary>
  /// <remarks>
  /// Clients who wish to trigger teardown may release the reference to the passed refTracker
  /// instance.  If this thread is the last thread holding a reference to this context, then
  /// after the point where the reference is released, [this] will be deleted.  Clients should
  /// be careful to hold their own references to refTracker before calling DoRun if they
  /// intend to reference member data on function return.  This method will always release the
  /// shared pointer passed to it, typically as a last step before return, potentially
  /// triggering the case described above.
  /// </remarks>
  virtual void DoRun(std::shared_ptr<Object>&& refTracker);

  /// <summary>
  /// Indicates that the system should accept the delivery of deferred procedure calls
  /// </summary>
  void AcceptDispatchDelivery(void) {
    boost::lock_guard<boost::mutex> lk(m_lock);
    m_canAccept = true;
    m_stateCondition.notify_all();
  }

  /// <summary>
  /// Indicates that no more deferred procedure calls should be pended
  /// </summary>
  /// <remarks>
  /// This method guarantees that, at some point after it returns, the caller will no
  /// longer receive pended events.  No guarantees are made about how long after this
  /// call is made that the last pended dispatch event will be delivered.
  ///
  /// Callers interested in this guarantee should invoke this method and then wait for
  /// the dispatch queue to become empty.
  ///
  /// This method is idempotent
  /// </remarks>
  void RejectDispatchDelivery(void) {
    boost::lock_guard<boost::mutex> lk(m_lock);
    m_canAccept = false;
    m_stateCondition.notify_all();
  }

public:
  // Override from DispatchQueue
  bool CanAccept(void) const override {return m_canAccept;}
  
  // Base overrides:
  bool DelayUntilCanAccept(void) override;

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
};
