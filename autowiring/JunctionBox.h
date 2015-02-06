// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "DispatchQueue.h"
#include "DispatchThunk.h"
#include "fast_pointer_cast.h"
#include "InvokeRelay.h"
#include "JunctionBoxBase.h"
#include "JunctionBoxEntry.h"
#include "TypeUnifier.h"
#include <set>
#include MEMORY_HEADER
#include TYPE_TRAITS_HEADER

class CoreContext;

/// <summary>
/// Utility routine which shuts down the current context
/// </summary>
void ShutdownCurrentContext(void);

struct NoType {};

template<class T>
class JunctionBox:
  public JunctionBoxBase
{
public:
  JunctionBox(void):
    m_numberOfDeletions(0)
  {}

  virtual ~JunctionBox(void) {}

protected:
  // Listener set lock
  mutable std::mutex m_lock;

  // Collection of all known listeners:
  typedef std::set<JunctionBoxEntry<T>> t_listenerSet;
  t_listenerSet m_st;

  // Incremented every time an event is deleted to notify potentially invalidated iterators
  volatile int m_numberOfDeletions;

public:
  /// <summary>
  /// Convenience method allowing consumers to quickly determine whether any listeners exist
  /// </summary>
  bool HasListeners(void) const override {
    return (std::lock_guard<std::mutex>)m_lock, !m_st.empty();
  }

  void Add(const JunctionBoxEntry<CoreObject>& rhs) override {
    auto casted = rhs.Rebind<T>();
    if(casted)
      // Proposed type is directly one of our receivers
      Add(casted);
  }

  void Remove(const JunctionBoxEntry<CoreObject>& rhs) override {
    auto casted = rhs.Rebind<T>();
    if(casted)
      Remove(casted);
  }

  /// <summary>
  /// Adds the specified observer to receive events raised from this instace
  /// </summary>
  void Add(const JunctionBoxEntry<T>& rhs) {
    std::lock_guard<std::mutex> lk(m_lock);

    // Trivial insert
    m_st.insert(rhs);
  }

  /// <summary>
  /// Removes the specified observer from the set currently configured to receive events
  /// </summary>
  void Remove(const JunctionBoxEntry<T>& rhs) {
    // Update the deletion count, delete, return
    std::lock_guard<std::mutex> lk(m_lock);
    m_numberOfDeletions++;
    m_st.erase(rhs);
  }

  /// <summary>
  /// Zero-argument deferred call relay
  /// </summary>
  /// <param name="fn">A nearly-curried routine to be invoked</param>
  /// <return>False if an exception was thrown by a recipient, true otherwise</return>
  template<class Fn, class... Args>
  bool FireCurried(const Fn& fn, Args&... args) const {
    std::unique_lock<std::mutex> lk(m_lock);
    int deleteCount = m_numberOfDeletions;

    // Collection of contexts that need to be torn down in the event of an exception.  Using a list,
    // here, instead of a vector because this list is expected to almost always be empty, and we can
    // avoid having to include <vector> by using a list instead.
    std::list<std::weak_ptr<CoreContext>> teardown;

    for (auto it = m_st.begin(); it != m_st.end();){
      JunctionBoxEntry<T> currentEvent(*it);

      lk.unlock();
      try {
        fn(*currentEvent.m_ptr, args...);
      }
      catch (...) {
        teardown.push_back(ContextDumbToWeak(currentEvent.m_owner));

        // If T doesn't inherit CoreObject, then we need to cast to a unifying type which does
        typedef typename SelectTypeUnifier<T>::type TActual;
        this->FilterFiringException(currentEvent.m_ptr);
      }
      lk.lock();

      // Increment iterator correctly even if it's been invalidated
      if (deleteCount == m_numberOfDeletions){
        ++it;
      }
      else {
        it = m_st.upper_bound(currentEvent);
        deleteCount = m_numberOfDeletions;
      }
    }
    if (teardown.empty())
      // Nobody threw any exceptions, end here
      return true;

    // Exceptions thrown, teardown and then indicate an error
    lk.unlock();
    TerminateAll(teardown);
    return false;
  }
};
