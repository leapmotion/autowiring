// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "dispatch_aborted_exception.h"
#include "DispatchThunk.h"
#include "once.h"
#include <atomic>
#include <list>
#include <queue>
#include MUTEX_HEADER
#include RVALUE_HEADER
#include MEMORY_HEADER

class DispatchQueue;

/// <summary>
/// This is an asynchronous queue of zero-argument functions
/// </summary>
/// <remarks>
/// A DispatchQueue is a type of event receiver which allows for the reception of deferred events.
/// </remarks>
class DispatchQueue {
public:
  DispatchQueue(void);
  DispatchQueue(size_t dispatchCap);
  DispatchQueue(DispatchQueue&&);
  DispatchQueue(const DispatchQueue&) = delete;

  /// <summary>
  /// Runs down the dispatch queue without calling anything
  /// </summary>
  /// <remarks>
  /// Nothing in the destructor is synchronized.  This is done under the assumption that multi-
  /// access during teardown is impossible.
  /// </remarks>
  virtual ~DispatchQueue(void);

  // True if DispatchQueue::Abort has been called.  This will cause the dispatch queue's remaining entries
  // to be dumped and prevent the introduction of new entries to the queue.
  autowiring::once_signal<DispatchQueue> onAborted;

protected:
  // The maximum allowed number of pended dispatches before pended calls start getting dropped
  size_t m_dispatchCap = 1024;

  // Current linked list length
  std::atomic<size_t> m_count{0};

  // Current version cap:
  std::atomic<uint64_t> m_version{1};

  // The dispatch queue proper.  A vector is used, here, not a queue, because this collection is frequently emptied.
  DispatchThunkBase* m_pHead = nullptr;
  DispatchThunkBase* m_pTail = nullptr;

  // Priority queue of non-ready events:
  std::priority_queue<DispatchThunkDelayed> m_delayedQueue;

  // A lock held when the dispatch queue must be updated:
  std::mutex m_dispatchLock;

  // Notice when the dispatch queue has been updated:
  std::condition_variable m_queueUpdated;

  /// <summary>
  /// Moves all ready events from the delayed queue into the dispatch queue
  /// </summary>
  /// <returns>True if at least one dispatcher was promoted</returns>
  bool PromoteReadyDispatchersUnsafe(void);

  /// <summary>
  /// Similar to DispatchEvent, except assumes that the dispatch lock is currently held
  /// </summary>
  /// <param name="lk">A lock on m_dispatchLock</param>
  /// <remarks>
  /// This method assumes that the dispatch lock is held and that m_aborted is false.  It
  /// is an error to call this method without those preconditions met.
  /// </remarks>
  void DispatchEventUnsafe(std::unique_lock<std::mutex>& lk);

  /// <summary>
  /// Utility virtual, called whenever a new event is deferred
  /// </summary>
  /// <remarks>
  /// The recipient of this call will be running in an arbitrary thread context while holding the dispatch
  /// lock.  The queue is guaranteed to contain at least one element, and may potentially contain more.  The
  /// caller MUST NOT attempt to pend any more events during this call, or a deadlock could occur.
  /// </remarks>
  virtual void OnPended(std::unique_lock<std::mutex>&& lk) {}

  template<class _Fx>
  void Pend(_Fx&& fx) {
    PendExisting(
      std::unique_lock<std::mutex>(m_dispatchLock),
      new DispatchThunk<_Fx>(std::forward<_Fx&&>(fx))
    );
  }

  /// <summary>
  /// Attaches an element to the end of the dispatch queue without any checks.
  /// </summary>
  void PendExisting(std::unique_lock<std::mutex>&& lk, DispatchThunkBase* thunk);

  /// <summary>
  /// Updates the upper bound on the number of allowed pending dispatchers
  /// </summary>
  void SetDispatcherCap(size_t dispatchCap) { m_dispatchCap = dispatchCap; }

public:
  /// <returns>
  /// True if there are curerntly any dispatchers ready for execution--IE, DispatchEvent would return true
  /// </returns>
  bool AreAnyDispatchersReady(void) const { return !!m_pHead; }

  /// <returns>
  /// The total number of all ready and delayed events
  /// </returns>
  /// <remarks>
  /// This method will also count dispatchers that are presently underway or presently being deleted.  Thus, calling
  /// this method from within a dispatcher, or from that dispatcher's destructor, should always return a size of at
  /// least 1.
  /// </remarks>
  size_t GetDispatchQueueLength(void) const {return m_count + m_delayedQueue.size();}

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
  /// Causes all calls to WaitForEvent to return control to their callers
  /// </summary>
  /// <remarks>
  /// This method will cause any threads blocked in WaitForEvent to wake up and make progress.  This can be
  /// useful when threads are being used to dispatch work items and it's necessary to wake them in order to
  /// handle out-of-queue processing--IE, pool maintenance
  /// </remarks>
  void WakeAllWaitingThreads(void);

  /// <summary>
  /// Similar to WaitForEvent, but does not block
  /// </summary>
  /// <returns>True if an event was dispatched, false if the queue was empty when checked</returns>
  /// <remarks>
  /// If the dispatch queue is empty, this method will check the delayed dispatch queue.
  /// </remarks>
  bool DispatchEvent(void);

  /// <summary>
  /// Similar to DispatchEvent, but will attempt to dispatch all events currently queued
  /// </summary>
  /// <returns>The total number of events dispatched</returns>
  int DispatchAllEvents(void);

  /// <summary>
  /// Waits until a lambda function is ready to run in this thread's dispatch queue,
  /// dispatches the function, and then returns.
  /// </summary>
  /// <remarks>
  /// This method will throw dispatch_aborted_exception if the queue has been aborted at the time of the call
  /// </remarks>
  void WaitForEvent(void);

  /// <summary>
  /// Waits until a lambda function in the dispatch queue is ready to run or the specified
  /// time period elapses, whichever comes first.
  /// </summary>
  /// <returns>
  /// False if the timeout period elapsed before an event could be dispatched, true otherwise
  /// </returns>
  bool WaitForEvent(std::chrono::milliseconds milliseconds);

  /// <summary>
  /// Waits until a lambda function in the dispatch queue is ready to run or the specified
  /// time is reached, whichever comes first.
  /// </summary>
  /// <returns>
  /// False if the timeout period elapsed before an event could be dispatched, true otherwise
  /// </returns>
  bool WaitForEvent(std::chrono::steady_clock::time_point wakeTime);

  /// \internal
  /// <summary>
  /// An unsafe variant of WaitForEvent
  /// </summary>
  bool WaitForEventUnsafe(std::unique_lock<std::mutex>& lk, std::chrono::steady_clock::time_point wakeTime);

  /// <summary>
  /// Explicit overload for already-constructed dispatch thunk types
  /// </summary>
  void AddExisting(std::unique_ptr<DispatchThunkBase>&& pBase) {
    std::unique_lock<std::mutex> lk(m_dispatchLock);
    if (m_count < m_dispatchCap)
      PendExisting(std::move(lk), pBase.release());
  }

  /// <summary>
  /// Blocks until all dispatchers on the DispatchQueue at the time of the call have been dispatched
  /// </summary>
  /// <param name="timeout">The maximum amount of time to wait</param>
  /// <remarks>
  /// This method does not cause any dispatchers to run.  If the underlying dispatch queue does not have an event loop
  /// operating on it, this method will deadlock.  It is an error for the party responsible for driving the dispatch queue
  /// via WaitForEvent or DispatchAllEvents unless that party first delegates the responsibility elsewhere.
  ///
  /// If DispatchQueue::Abort() is called before the dispatcher has been completed, this method will throw an exception.
  /// If a dispatcher on the underlying DispatchQueue throws an exception, this method will also throw an exception.
  ///
  /// If zero is passed as the timeout value, this method will return true if and only if the queue was empty at the time
  /// of the call, ignoring any delayed dispatchers.
  /// </remarks>
  bool Barrier(std::chrono::nanoseconds timeout);

  /// <summary>
  /// Identical to the timed version of Barrier, but does not time out
  /// </summary>
  void Barrier(void);

  /// <summary>
  /// Recommends a point in time to wake up to check for events
  /// </summary>
  /// <returns>
  /// lastestTime, or if there is a dispatch in the delayed queue which will be ready sooner than latestTime, the
  /// time_point when that dispatch will be ready to run.
  /// </returns>
  /// <remarks>
  /// This method is used by clients that are performing manual dispatch operations and wish to know the shortest time
  /// they should sleep in order to be guaranteed that, upon waking, a dispatch will be ready to run.
  ///
  /// Users should be aware that another dispatch may arrive which is ready to run sooner than the returned suggestion.
  /// Notification of this case will be provided to derived classes via the OnPended override.
  /// </remarks>
  std::chrono::steady_clock::time_point SuggestSoonestWakeupTimeUnsafe(std::chrono::steady_clock::time_point latestTime) const;

  class DispatchThunkDelayedExpressionRel {
  public:
    DispatchThunkDelayedExpressionRel(DispatchQueue* pParent, std::chrono::microseconds delay) :
      m_pParent(pParent),
      m_delay(delay)
    {}

  private:
    DispatchQueue* const m_pParent;
    const std::chrono::microseconds m_delay;

  public:
    template<class _Fx>
    void operator,(_Fx&& fx) {
      // Let the parent handle this one directly after composing a delayed dispatch thunk r-value
      if (m_delay.count())
        *m_pParent += DispatchThunkDelayed(
          std::chrono::steady_clock::now() + m_delay,
          new DispatchThunk<_Fx>(std::forward<_Fx&&>(fx))
        );
      else
        *m_pParent += std::forward<_Fx&&>(fx);
    }
  };

  class DispatchThunkDelayedExpressionAbs {
  public:
    DispatchThunkDelayedExpressionAbs(DispatchQueue* pParent, std::chrono::steady_clock::time_point wakeup) :
      m_pParent(pParent),
      m_wakeup(wakeup)
    {}

  private:
    DispatchQueue* const m_pParent;
    const std::chrono::steady_clock::time_point m_wakeup;

  public:
    template<class _Fx>
    void operator,(_Fx&& fx) {
      // Let the parent handle this one directly after composing a delayed dispatch thunk r-value
      *m_pParent += DispatchThunkDelayed(
        m_wakeup,
        new DispatchThunk<_Fx>(std::forward<_Fx>(fx))
      );
    }
  };

  /// <summary>
  /// Extracts the contents of the dispatch queue on the right-hand side for handling by this queue
  /// </summary>
  void operator+=(DispatchQueue&& rhs);

  /// <summary>
  /// Overload for the introduction of a delayed dispatch thunk
  /// </summary>
  /// <remarks>
  /// If the passed duration is equal to zero, the returned expression template will pend a lambda
  /// to the dispatch queue as though that lambda were added with operator+= without any delay.
  /// </remarks>
  template<class Rep, class Period>
  DispatchThunkDelayedExpressionRel operator+=(std::chrono::duration<Rep, Period> rhs) {
    // Verify that the duration is at least microseconds.  If you're getting an assertion here, try
    // using std::duration_cast<std::chrono::microseconds>(duration)
    static_assert(
      Period::num / Period::den <= 1000000,
      "Dispatch queues cannot be used to describe intervals less than one microseconds in duration"
    );

    return{this, rhs};
  }

  /// <summary>
  /// Overload for absolute-time based delayed dispatch thunk
  /// </summary>
  DispatchThunkDelayedExpressionAbs operator+=(std::chrono::steady_clock::time_point rhs);

  /// <summary>
  /// Directly pends a delayed dispatch thunk
  /// </summary>
  /// <remarks>
  /// This overload will always succeed and does not consult the dispatch cap
  /// </remarks>
  void operator+=(DispatchThunkDelayed&& rhs);

  /// <summary>
  /// Generic overload which will pend an arbitrary dispatch type
  /// </summary>
  template<class _Fx>
  void operator+=(_Fx&& fx) {
    static_assert(!std::is_base_of<DispatchThunkBase, _Fx>::value, "Overload resolution malfunction, must not doubly wrap a dispatch thunk");
    static_assert(!std::is_pointer<_Fx>::value, "Cannot pend a pointer to a function, we must have direct ownership");

    // Create the thunk first to reduce the amount of time we spend in lock:
    auto thunk = new DispatchThunk<_Fx>(std::forward<_Fx>(fx));

    m_dispatchLock.lock();
    if (m_count >= m_dispatchCap) {
      m_dispatchLock.unlock();
      delete thunk;
      return;
    }

    // Count must be separately maintained:
    m_count++;

    // Linked list setup:
    if (m_pHead) {
      m_pTail->m_pFlink = thunk;
      m_pTail = thunk;
      m_dispatchLock.unlock();
    }
    else {
      m_pHead = m_pTail = thunk;
      m_dispatchLock.unlock();
      m_queueUpdated.notify_all();
    }

    // Notification as needed:
    OnPended(std::unique_lock<std::mutex>{});
  }
};
