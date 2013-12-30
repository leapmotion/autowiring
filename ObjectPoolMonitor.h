#pragma once
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

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
  public boost::mutex
{
public:
  ObjectPoolMonitor(void);

private:
  bool m_abandoned;

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