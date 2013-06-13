#ifndef _LOCK_REDUCED_COLLECTION_H
#define _LOCK_REDUCED_COLLECTION_H
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
/// </remarks>
template<class T, class Hash>
class LockReducedCollection
{
public:
  class Collection:
    public std::unordered_set<T, Hash>
  {
  public:
    // Interior shared pointer, manually released in order to cause destruction:
    mutable std::shared_ptr<Collection> m_circular;
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

  // Buffer pool:
  ObjectPool<Collection> m_pool;

public:
  /// <summary>
  /// Obtains a lock-free image of the current buffer
  /// </summary>
  t_mpTypeConst GetImage(void) const {
    // Spin lock:
    Collection* primary;
    do primary = (Collection*)exchange_acquire((void*volatile*)&m_primary, nullptr);
    while(!primary);

    // Copy out the return value:
    t_mpTypeConst retVal = primary->m_circular;

    // Spin lock exit:
    auto pPrior = exchange_release((void*volatile*)&m_primary, primary);
    assert(!pPrior);

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
    while(pPrimary != compare_exchange((void*volatile*)&m_primary, pPrimary, emptyPtr));

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
    t_mpType secondary;
    m_pool(secondary);
    secondary->m_circular = secondary;

    // Store the item we're going to be performing all of our operations on:
    t_mpTypeConst primary;
    do {
      // Test, first, because then we can drop an O(n):
      primary = GetImage();
      if(primary->count(value))
        return;

      // Create a copy and insert:
      *secondary = *primary;
      secondary->insert(value);

      // Check-and-set:
    } while(primary.get() != compare_exchange((void*volatile*)&m_primary, secondary.get(), primary.get()));

    // Graceful release:
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
    t_mpType secondary;
    m_pool(secondary);
    secondary->m_circular = secondary;
    
    t_mpTypeConst primary;
    do {
      // Test, first, because then we can drop an O(n):
      primary = GetImage();

      // Create a copy and drop:
      *secondary = *primary;
      secondary->erase(value);

      // Check-and-set:
    } while(primary.get() != compare_exchange((void*volatile*)&m_primary, secondary.get(), primary.get()));
    return false;
  }
};

#endif