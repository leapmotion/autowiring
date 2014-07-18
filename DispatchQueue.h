#pragma once

#include "EventReceiver.h"
#include "DispatchThunk.h"
#include <list>
#include <queue>
#include MUTEX_HEADER
#include RVALUE_HEADER
#include MEMORY_HEADER

class DispatchQueue;

/// <summary>
/// Thrown when a dispatch operation was aborted
/// </summary>
class dispatch_aborted_exception:
  public std::exception
{};

/// <summary>
/// This is an asynchronous queue of zero-argument functions
/// </summary>
/// <remarks>
/// A DispatchQueue is a type of event receiver which allows for the reception of deferred events.
/// </remarks>
class DispatchQueue:
  public virtual EventReceiver
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
  size_t m_dispatchCap;

  // The dispatch queue proper:
  std::list<DispatchThunkBase*> m_dispatchQueue;

  // Priority queue of non-ready events:
  std::priority_queue<DispatchThunkDelayed> m_delayedQueue;

  // A lock held when the dispatch queue must be updated:
  std::mutex m_dispatchLock;

  // Notice when the dispatch queue has been updated:
  std::condition_variable m_queueUpdated;

  bool m_aborted;

  /// <summary>
  /// Recommends a point in time to wake up to check for events
  /// </summary>
  std::chrono::steady_clock::time_point SuggestSoonestWakeupTimeUnsafe(std::chrono::steady_clock::time_point latestTime) const;

  /// <summary>
  /// Moves all ready events from the delayed queue into the dispatch queue
  /// </summary>
  void PromoteReadyEventsUnsafe(void);

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

  /// <summary>
  /// Attaches an element to the end of the dispatch queue without any checks.
  /// </summary>
  template<class _Fx>
  void Pend(_Fx&& fx) {
    std::unique_lock<std::mutex> lk(m_dispatchLock);
    m_dispatchQueue.push_back(new DispatchThunk<_Fx>(fx));
    m_queueUpdated.notify_all();

    OnPended(std::move(lk));
  }

public:
  /// <returns>
  /// True if there are curerntly any dispatchers ready for execution--IE, DispatchEvent would return true
  /// </returns>
  bool AreAnyDispatchersReady(void) const { return !m_dispatchQueue.empty(); }

  /// <returns>
  /// The total number of all ready and delayed events
  /// </returns>
  size_t GetDispatchQueueLength(void) const {return m_dispatchQueue.size() + m_delayedQueue.size();}

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

protected:
  /// <summary>
  /// Updates the upper bound on the number of allowed pending dispatchers
  /// </summary>
  void SetDispatcherCap(size_t dispatchCap) { m_dispatchCap = dispatchCap; }

  /// <summary>
  /// Similar to WaitForEvent, but does not block
  /// </summary>
  /// <returns>True if an event was dispatched, false if the queue was empty when checked</returns>
  bool DispatchEvent(void);

  /// <summary>
  /// Similar to DispatchEvent, but will attempt to dispatch all events currently queued
  /// </summary>
  /// <returns>The total number of events dispatched</returns>
  int DispatchAllEvents(void) {
    int retVal = 0;
    while(DispatchEvent())
      retVal++;
    return retVal;
  }

public:
  /// <summary>
  /// Explicit overload for already-constructed dispatch thunk types
  /// </summary>
  void AddExisting(DispatchThunkBase* pBase) {
    std::unique_lock<std::mutex> lk(m_dispatchLock);
    if(m_dispatchQueue.size() >= m_dispatchCap)
      return;

    m_dispatchQueue.push_back(pBase);
    m_queueUpdated.notify_all();
    OnPended(std::move(lk));
  }

  class DispatchThunkDelayedExpression {
  public:
    DispatchThunkDelayedExpression(DispatchQueue* pParent, std::chrono::steady_clock::time_point wakeup) :
      m_pParent(pParent),
      m_wakeup(wakeup)
    {}

  private:
    DispatchQueue* m_pParent;
    std::chrono::steady_clock::time_point m_wakeup;

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
  /// Overload for the introduction of a delayed dispatch thunk
  /// </summary>
  template<class Rep, class Period>
  DispatchThunkDelayedExpression operator+=(std::chrono::duration<Rep, Period> rhs) {
    // Verify that the duration is at least microseconds.  If you're getting an assertion here, try
    // using std::duration_cast<std::chrono::microseconds>(duration)
    static_assert(
      Period::num / Period::den <= 1000000,
      "Dispatch queues cannot be used to describe intervals less than one microseconds in duration"
    );

    std::chrono::steady_clock::time_point timepoint = std::chrono::steady_clock::now() + rhs;
    return *this += timepoint;
  }

  /// <summary>
  /// Overload for absolute-time based delayed dispatch thunk
  /// </summary>
  DispatchThunkDelayedExpression operator+=(std::chrono::steady_clock::time_point rhs) {
    return DispatchThunkDelayedExpression(this, rhs);
  }

  /// <summary>
  /// Directly pends a delayed dispatch thunk
  /// </summary>
  /// <remarks>
  /// This overload will always succeed and does not consult the dispatch cap
  /// </remarks>
  void operator+=(DispatchThunkDelayed&& rhs) {
    std::lock_guard<std::mutex> lk(m_dispatchLock);

    m_delayedQueue.push(std::forward<DispatchThunkDelayed>(rhs));
    if(
      m_delayedQueue.top().GetReadyTime() == rhs.GetReadyTime() &&
      m_dispatchQueue.empty()
    )
      // We're becoming the new next-to-execute entity, dispatch queue currently empty, trigger wakeup
      // so our newly pended delay thunk is eventually processed.
      m_queueUpdated.notify_all();
  }

  /// <summary>
  /// Generic overload which will pend an arbitrary dispatch type
  /// </summary>
  template<class _Fx>
  void operator+=(_Fx&& fx) {
    static_assert(!std::is_base_of<DispatchThunkBase, _Fx>::value, "Overload resolution malfunction, must not doubly wrap a dispatch thunk");
    static_assert(!std::is_pointer<_Fx>::value, "Cannot pend a pointer to a function, we must have direct ownership");

    std::unique_lock<std::mutex> lk(m_dispatchLock);
    if(m_dispatchQueue.size() >= m_dispatchCap)
      return;

    m_dispatchQueue.push_back(new DispatchThunk<_Fx>(std::forward<_Fx>(fx)));
    m_queueUpdated.notify_all();
    OnPended(std::move(lk));
  }
};

