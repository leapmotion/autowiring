// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _DISPATCH_QUEUE_H
#define _DISPATCH_QUEUE_H
#include "ocuConfig.h"
#include "EventDispatcher.h"
#include "EventReceiver.h"
#include "DispatchThunk.h"
#include "DispatchThunkEventProxy.h"
#include <boost/thread/condition_variable.hpp>
#include FUNCTIONAL_HEADER
#include RVALUE_HEADER
#include <list>

class DispatchQueue;

/// <summary>
/// Thrown when a dispatch operation was aborted
/// </summary>
class dispatch_aborted_exception:
  public std::exception
{
};

/// <summary>
/// This is an asynchronous queue of zero-argument functions
/// </summary>
/// <remarks>
/// A DispatchQueue is a type of event receiver which allows for the reception of deferred events.
/// </remarks>
class DispatchQueue:
  public virtual EventReceiver,
  public EventDispatcher
{
public:
  DispatchQueue(void);

  /// <summary>
  /// Runs down the dispatch queue without calling anything
  /// </summary>
  /// <remarks>
  /// Nothing in the destructor is synchronized.  This is done under the assumption that multi-
  /// access during teardown is impossible.
  /// </remarks>
  virtual ~DispatchQueue(void);

protected:
  // The maximum allowed number of pended dispatches before pended calls start getting dropped
  int m_dispatchCap;

private:
  bool m_aborted;

  // A lock held when modifications to any element EXCEPT the first element must be made:
  boost::mutex m_dispatchLock;

  // Notice when the dispatch queue has been updated:
  boost::condition_variable m_queueUpdated;

  // The dispatch queue proper:
  std::list<DispatchThunkBase*> m_dispatchQueue;

protected:
  /// <summary>
  /// Similar to DispatchEvent, except assumes that the dispatch lock is currently held
  /// </summary>
  /// <param name="lk">A lock on m_dispatchLock</param>
  /// <remarks>
  /// This method assumes that the dispatch lock is held and that m_aborted is false.  It
  /// is an error to call this method without those preconditions met.
  /// </remarks>
  void DispatchEventUnsafe(boost::unique_lock<boost::mutex>& lk);

  /// <summary>
  /// Utility virtual, called whenever a new event is deferred
  /// </summary>
  /// <remarks>
  /// The recipient of this call will be running in an arbitrary thread context while holding the dispatch
  /// lock.  The queue is guaranteed to contain at least one element, and may potentially contain more.  The
  /// caller MUST NOT attempt to pend any more events during this call, or a deadlock could occur.
  /// </remarks>
  virtual void OnPended(void) {}

  /// <summary>
  /// Attaches an element to the end of the dispatch queue without any checks.
  /// </summary>
  template<class _Fx>
  void Pend(_Fx&& fx) {
    boost::lock_guard<boost::mutex> lk(m_dispatchLock);
    m_dispatchQueue.push_back(new DispatchThunk<_Fx>(fx));
    m_queueUpdated.notify_all();

    OnPended();
  }
  
public:
  // Accessor methods:
  size_t GetDispatchQueueLength(void) const {return m_dispatchQueue.size();}

  /// <summary>
  /// Causes the current dispatch queue to be dumped if it's non-empty
  /// </summary>
  /// <remarks>
  /// This method should only be called if a non-graceful termination is desired.  In this case, the dispatch
  /// queue will be immediately cleared and any subsequent calls to WaitForEvent or DispatchEvent will throw
  /// a dispatch_aborted_exception.
  ///
  /// Callers who are willing to allow the dispatch queue to be fully processed should call Rundown instead.
  ///
  /// This method is idempotent
  /// </remarks>
  void Abort(void);

  /// <summary>
  /// Blocks until a new dispatch member is added, dispatches, and then returns
  /// </summary>
  void WaitForEvent(void) override;

  /// <summary>
  /// Timed version of WaitForEvent
  /// </summary>
  /// <returns>
  /// False if the timeout period elapsed before an event could be dispatched, true otherwise
  /// </returns>
  bool WaitForEvent(boost::chrono::duration<double, boost::milli> milliseconds);

  /// <summary>
  /// Wakeup-point version of WaitForEvent
  /// </summary>
  /// <returns>
  /// False if the timeout period elapsed before an event could be dispatched, true otherwise
  /// </returns>
  bool WaitForEvent(boost::chrono::steady_clock::time_point wakeTime);

  /// <summary>
  /// Similar to WaitForEvent, but does not block
  /// </summary>
  /// <returns>True if an event was dispatched, false if the queue was empty when checked</returns>
  bool DispatchEvent(void) override;

  /// <summary>
  /// Similar to DispatchEvent, but will attempt to dispatch all events currently queued
  /// </summary>
  /// <returns>The total number of events dispatched</returns>
  int DispatchAllEvents(void) override {
    int retVal = 0;
    while(DispatchEvent())
      retVal++;
    return retVal;
  }

  /// <summary>
  /// Dispatcher overload, expressly for use with types which expect an event
  /// </summary>
  /// <param name="eventProxy">A proxy routine to the destination event type</param>
  /// <remarks>
  /// This overload is intended for use where an event call must be made on a partially bound destination.
  /// The passed call will receive a pointer to this DispatchQueue, and will be expected to cast it to the
  /// correct destination type prior to actually making the call.
  ///
  /// The event receiver function must be idempotent, and must be callable on types other than [this].
  /// Certain derived implementations may proxy the event call, sending it elsewhere, possibly more than
  /// once, which requires that the passed routine be invariant.
  /// </remarks>
  virtual void AttachProxyRoutine(std::function<void (EventReceiver&)>&& eventProxy) {
    boost::lock_guard<boost::mutex> lk(m_dispatchLock);
    m_dispatchQueue.push_back(new DispatchThunkEventProxy(*this, std::move(eventProxy)));
    m_queueUpdated.notify_all();
  }

  /// <summary>
  /// Generic overload which will pend an arbitrary dispatch type
  /// </summary>
  template<class _Fx>
  void operator+=(_Fx&& fx) {
    boost::lock_guard<boost::mutex> lk(m_dispatchLock);
    if(!CanAccept())
      return;

    if(static_cast<int>(m_dispatchQueue.size()) > m_dispatchCap)
      return;

    m_dispatchQueue.push_back(new DispatchThunk<_Fx>(fx));
    m_queueUpdated.notify_all();
    OnPended();
  }
};

#endif
