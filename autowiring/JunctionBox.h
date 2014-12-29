// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "DispatchQueue.h"
#include "DispatchThunk.h"
#include "fast_pointer_cast.h"
#include "InvokeRelay.h"
#include "JunctionBoxBase.h"
#include "JunctionBoxEntry.h"
#include "TypeUnifier.h"
#include <set>
#include STL_TUPLE_HEADER
#include RVALUE_HEADER
#include MEMORY_HEADER
#include STL_UNORDERED_SET
#include TYPE_TRAITS_HEADER

class CoreContext;
class JunctionBoxBase;

template<typename T>
struct JunctionBoxEntry;

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

  void Add(const JunctionBoxEntry<Object>& rhs) override {
    auto casted = rhs.Rebind<T>();
    if(casted)
      // Proposed type is directly one of our receivers
      Add(casted);
  }

  void Remove(const JunctionBoxEntry<Object>& rhs) override {
    auto casted = rhs.Rebind<T>();
    if(casted)
      Remove(casted);
  }

  /// <summary>
  /// Adds the specified observer to receive events dispatched from this instace
  /// </summary>
  void Add(const JunctionBoxEntry<T>& rhs) {
    std::lock_guard<std::mutex> lk(m_lock);

    // Trivial insert
    m_st.insert(rhs);

    // If the RHS implements DispatchQueue, add it to that collection as well:
    DispatchQueue* pDispatch = autowiring::fast_pointer_cast<DispatchQueue, T>(rhs.m_ptr).get();
    if(pDispatch)
      m_dispatch.insert(pDispatch);
  }

  /// <summary>
  /// Removes the specified observer from the set currently configured to receive events
  /// </summary>
  void Remove(const JunctionBoxEntry<T>& rhs) {
    std::lock_guard<std::mutex> lk(m_lock);

    // Update the deletion count
    m_numberOfDeletions++;

    // If the RHS implements DispatchQueue, remove it from the dispatchers collection
    DispatchQueue* pDispatch = autowiring::fast_pointer_cast<DispatchQueue, T>(rhs.m_ptr).get();
    if(pDispatch)
      m_dispatch.erase(pDispatch);

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

    // Set of contexts that need to be torn down in the event of an exception:
    std::vector<std::weak_ptr<CoreContext>> teardown;

    for(auto it = m_st.begin(); it != m_st.end(); ){
      JunctionBoxEntry<T> currentEvent(*it);

      lk.unlock();
      try {
        fn(*currentEvent.m_ptr, args...);
      } catch(...) {
        teardown.push_back(ContextDumbToWeak(currentEvent.m_owner));
        
        // If T doesn't inherit Object, then we need to cast to a unifying type which does
        typedef typename SelectTypeUnifier<T>::type TActual;
        this->FilterFiringException(autowiring::fast_pointer_cast<TActual>(currentEvent.m_ptr));
      }
      lk.lock();

      // Increment iterator correctly even if it's been invalidated
      if (deleteCount == m_numberOfDeletions){
        ++it;
      } else {
        it = m_st.upper_bound(currentEvent);
        deleteCount = m_numberOfDeletions;
      }
    }
    if(teardown.empty())
      // Nobody threw any exceptions, end here
      return true;

    // Exceptions thrown, teardown and then indicate an error
    lk.unlock();
    TerminateAll(teardown);
    return false;
  }
};
