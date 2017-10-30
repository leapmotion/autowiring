// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include "InterlockedExchange.h"
#include "ObjectPool.h"
#include STL_UNORDERED_SET
#include <assert.h>

/// <summary>
/// Implements a lock-free collection
/// </summary>
/// <remarks>
/// A reduced-lock collection is a collection where there are no locks used to restrict
/// read access, and write access is performed with optimistic locking.  The lock reduced
/// collection has very inefficient insertion, but very efficient search.
///
/// This collection is implemented with spin locks to make it more lightweight.  If the
/// collection becomes highly contended or is frequently edited, changes will be necessary.
/// </remarks>
template<class T, class Hash>
class LockReducedCollection
{
public:
  /// <summary>
  /// Implements a simple immutable collection.
  /// </summary>
  /// <remarks>
  /// This collection is not altered after it is made the current primary collection.
  /// This enables multisynchronous access and enumeration on the collection itself,
  /// while the parent container provides a way to update the current constant collection.
  /// </remarks>
  class Collection:
    public std::unordered_set<T, Hash>
  {
  public:
    ~Collection(void) {
    }

    // Interior shared pointer, manually released in order to cause destruction:
    mutable std::shared_ptr<Collection> m_circular;

    // Use the base assignment implementation:
    Collection& operator=(const Collection& rhs) {
      this->insert(rhs.begin(), rhs.end());
      return *this;
    }

    void Reset(void) {
      std::unordered_set<T, Hash>::clear();
    }

    // TODO:  If this datastructure is too slow, then a singly linked list should be
    // placed here to indicate the operations to be conducted on this collection.
    //
    // Once the collection is released back to the pool, the associated Reset method
    // should be used to update this collection according to the cached sequence of
    // operations.  This enables collections that are returned to the object pool
    // to converge to the current active collection, and prevents new consumers from
    // having to copy the entire contents of the current collection when preparing
    // to replace it or during an optimistic lock failure.
  };

  LockReducedCollection(void) {
    // Obtain a collection pointer.  Circular reference will keep it resident.
    std::shared_ptr<Collection> collection;
    m_pool(collection);
    collection->m_circular = collection;
    m_primary = collection.get();
  }

  ~LockReducedCollection(void) {
    // Interior reset, this will trigger destruction:
    std::shared_ptr<Collection> circular;

    Collection* primary = (Collection*)m_primary;
    primary->m_circular.swap(circular);
  }

private:
  typedef std::shared_ptr<Collection> t_mpType;
  typedef std::shared_ptr<const Collection> t_mpTypeConst;

  // The primary buffer:
  mutable Collection* volatile m_primary;

  struct Resetter {
    void operator() (Collection& op) {
      op.Reset();
    }
  };

  // Buffer pool:
  ObjectPool<Collection, Resetter> m_pool;

  /// <summary>
  /// Acquires the current m_primary value, and sets the m_primary value to nullptr
  /// </summary>
  Collection* Acquire(void) const {
    // Spin lock:
    Collection* primary;
    do primary = (Collection*)exchange_acquire((void*volatile*)&m_primary, nullptr);
    while(!primary);
    return primary;
  }

  /// <summary>
  /// Provides a new collection to replace the current primary collection
  /// </summary>
  Collection* Propose(void) {
    std::shared_ptr<Collection> retVal;
    m_pool(retVal);
    retVal->m_circular = retVal;
    return retVal.get();
  }

  void Release(Collection* primary) const {
    // Spin lock exit:
    auto pPrior = exchange_release((void*volatile*)&m_primary, primary);
    (void)pPrior;

    // The only time this assert might fail is if someone attempts to release a spin lock
    // they do not own.
    // NOTE:  The thread releasing the unowned spin lock is not necessarily the one that
    // hits this assert!
    assert(!pPrior);
  }

public:
  /// <summary>
  /// Obtains a lock-free image of the current buffer
  /// </summary>
  t_mpTypeConst GetImage(void) const {
    // Spin lock:
    Collection* primary = Acquire();

    // Copy out the return value:
    t_mpTypeConst retVal = primary->m_circular;

    // Spin lock exit:
    Release(primary);

    // Done
    return retVal;
  }

  /// <summary>
  /// Replaces the primary collection with an empty collection
  /// </summary>
  void Clear(void) {
    // Empty pool generation first:
    t_mpType empty;
    m_pool(empty);
    empty->m_circular = empty;
    auto emptyPtr = empty.get();

    // Exchange out:
    Collection* pPrimary;
    do
      do pPrimary = m_primary;
      while(!pPrimary);
    while(pPrimary != compare_exchange((void*volatile*)&m_primary, emptyPtr, pPrimary));

    // Allow the primary element to return to the object pool:
    pPrimary->m_circular.reset();
  }

  /// <summary>
  /// Inserts the passed value into the current buffer
  /// </summary>
  /// <remarks>
  /// O(n) insertion efficiency.  Requires a memory allocation.
  /// </remarks>
  void Insert(const T& value) {
    // Create the destination type:
    auto secondary = Propose();

    // Spin lock:
    Collection* primary = Acquire();

    // Test, first, because then we can drop an O(n):
    if(primary->count(value))
      std::swap(primary, secondary);
    else {
      // Create a copy and insert:
      *secondary = *primary;
      secondary->insert(value);
    }

    // Release:
    Release(secondary);

    // Primary container back to collection:
    primary->m_circular.reset();
  }

  /// <summary>
  /// Inserts the passed value into the current buffer
  /// </summary>
  /// <returns>True if an element was deleted, false otherwise</returns>
  /// <remarks>
  /// O(n) deletion efficiency.  Requires a memory allocation.
  /// </remarks>
  bool Erase(const T& value) {
    // Create the destination type:
    auto secondary = Propose();

    // Spin lock:
    auto primary = Acquire();

    // Create a copy and erase:
    *secondary = *primary;
    secondary->erase(value);

    // Release:
    Release(secondary);

    // Pointer free:
    primary->m_circular.reset();
    return false;
  }
};
