#pragma once
#include <boost/thread/mutex.hpp>

template<class T>
class ObjectPool;

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
  /// <param name="pOwner">The owner of this object pool monitor</param>
  ObjectPoolMonitor(void* pOwner);

private:
  void* m_pOwner;
  bool m_abandoned;

public:
  // Accessor methods:
  void* GetOwner(void) const { return m_pOwner; }

  // Mutator methods:
  void SetOwner(void* pOwner) { m_pOwner = pOwner; }

  /// <return>
  /// True if this pool has been abandoned
  /// </return>
  bool IsAbandoned(void) const { return m_abandoned; }

  /// <summary>
  /// Transitions this state keeper to the abandoned state, causing all outstanding shared pointers to be destroyed
  /// </remarks>
  void Abandon(void);
};
