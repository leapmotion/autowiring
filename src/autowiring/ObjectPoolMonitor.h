// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include <functional>
#include MUTEX_HEADER

template<class T>
class ObjectPool;

namespace autowiring {

/// <summary>
/// Interior state object of the object pool, provided to allow out-of-order teardown on the object pool
/// </summary>
/// <remarks>
/// The object pool holds the sole permanent shared pointer to an instance of this object.  When the pool
/// marks the pool state as abandoned, outstanding shared pointers will no longer attempt to return to the
/// pool, and will instead free themselves directly.
///
/// A separate pool state is required because object pools are designed to be embedded in other objects,
/// which prevents teardown responsibility from being deferred.
/// </remarks>
class ObjectPoolMonitor:
  public std::mutex
{
public:
  ObjectPoolMonitor(void);

private:
  bool m_abandoned = false;

public:
  /// <return>
  /// True if this pool has been abandoned
  /// </return>
  bool IsAbandoned(void) const { return m_abandoned; }

  /// <summary>
  /// Transitions this state keeper to the abandoned state, causing all outstanding shared pointers to be destroyed
  /// </remarks>
  void Abandon(void);
};

template<typename T>
class ObjectPoolMonitorT:
  public ObjectPoolMonitor
{
public:
  ObjectPoolMonitorT(ObjectPool<T>* owner) :
    owner(owner),
    initial([](T&) {}),
    fnl([](T&) {})
  {}
  ObjectPoolMonitorT(ObjectPool<T>* owner, std::function<void(T&)> initial, std::function<void(T&)> fnl) :
    owner(owner),
    initial(initial),
    fnl(fnl)
  {}

  // Owner back-reference
  ObjectPool<T>* owner;

  // Resetters:
  const std::function<void(T&)> initial;
  const std::function<void(T&)> fnl;
};

}
