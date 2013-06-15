// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _DISPATCH_QUEUE_H
#define _DISPATCH_QUEUE_H
#include "ocuConfig.h"
#include "EventDispatcher.h"
#include <boost/thread/condition_variable.hpp>
#include <functional>
#include <list>

/// <summary>
/// Thrown when a dispatch operation was aborted
/// </summary>
class dispatch_aborted_exception:
  public std::exception
{
};

/// <summary>
/// A simple virtual class used to hold a trivial thunk
/// </summary>
class DispatchThunkBase {
public:
  virtual ~DispatchThunkBase(void) {}
  virtual void operator()() = 0;
};

template<class _Fx>
class DispatchThunk:
  public DispatchThunkBase
{
public:
  DispatchThunk(const _Fx& fx):
    m_fx(fx)
  {}

  _Fx m_fx;

  void operator()() override {
    m_fx();
  }
};

/// <summary>
/// This is an asynchronous queue of zero-argument functions
/// </summary>
class DispatchQueue:
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

  /// <summary>
  /// Similar to DispatchEvent, except assumes that the dispatch lock is currently held
  /// </summary>
  /// <param name="lk">A lock on m_dispatchLock</param>
  /// <remarks>
  /// This method assumes that the dispatch lock is held and that m_aborted is false.  It
  /// is an error to call this method without those preconditions met.
  /// </remarks>
  void DispatchEventUnsafe(boost::unique_lock<boost::mutex>& lk);

public:
  /// <summary>
  /// Causes all currently blocked thread members to quit and prevents any blocking
  /// </summary>
  void Abort(void);

  /// <summary>
  /// Blocks until a new dispatch member is added, dispatches, and then returns
  /// </summary>
  void WaitForEvent(void) override;

  /// <summary>
  /// Similar to WaitForEvent, but does not block
  /// </summary>
  /// <returns>True if an event was dispatched, false if the queue was empty when checked</returns>
  bool DispatchEvent(void) override;

  /// <summary>
  /// Similar to DispatchEvent, but will attempt to dispatch all events currently queued
  /// </summary>
  /// <returns>The total number of events dispatched</returns>
  size_t DispatchAllEvents(void) {
    size_t retVal = 0;
    while(DispatchEvent())
      retVal++;
    return retVal;
  }

  template<class _Fx>
  void operator+=(_Fx&& fx) {
    boost::lock_guard<boost::mutex> lk(m_dispatchLock);
    if(static_cast<int>(m_dispatchQueue.size()) > m_dispatchCap)
      return;

    m_dispatchQueue.push_back(new DispatchThunk<_Fx>(fx));
    m_queueUpdated.notify_all();
  }
};

#endif
