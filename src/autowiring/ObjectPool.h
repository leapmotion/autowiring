// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include "autowiring_error.h"
#include "ObjectPoolMonitor.h"
#include <cassert>
#include <vector>
#include FUNCTIONAL_HEADER
#include RVALUE_HEADER
#include MEMORY_HEADER

template<typename T>
void DefaultPlacement(T* ptr) { new(ptr) T; }

template<typename T>
void DefaultInitialize(T&){}

template<typename T>
void DefaultFinalize(T&){}

namespace autowiring {
  struct placement_t {};
  static const placement_t placement{};
}

/// <summary>
/// Allows the management of a pool of objects based on an embedded factory.
/// </summary>
/// <param name="T>The type to be pooled.</param>
/// <remarks>
/// This class is a type of factory that creates an object of type T.  The object pool
/// creates a shared pointer for the consumer to use, and when the last shared pointer
/// for that object is destroyed, the wrapped object is returned to this pool rather than
/// being deleted.  Returned objects may satisfy subsequent requests for construction.
///
/// All object pool methods are thread safe.
///
/// Issued pool members must be released before the pool goes out of scope.
/// </remarks>
template<class T>
class ObjectPool
{
public:
  ObjectPool(void) :
    m_monitor(std::make_shared<autowiring::ObjectPoolMonitorT<T>>(this, &DefaultInitialize<T>, &DefaultFinalize<T>)),
    m_maxPooled(~0),
    m_limit(~0)
  {}

  ObjectPool(ObjectPool&& rhs)
  {
    *this = std::move(rhs);
  }

  ObjectPool(
    size_t limit,
    size_t maxPooled = ~0,
    const std::function<void(T*)>& placement = &DefaultPlacement<T>,
    const std::function<void(T&)>& initial = &DefaultInitialize<T>,
    const std::function<void(T&)>& final = &DefaultFinalize<T>
  ) :
    m_monitor(std::make_shared<autowiring::ObjectPoolMonitorT<T>>(this, initial, final)),
    m_maxPooled(maxPooled),
    m_limit(limit),
    m_placement(placement)
  {};

  /// <param name="placement">
  /// A placement constructor to be used on the memory allocated for objects
  /// </param>
  /// <param name="maxPooled">
  /// The initializer that will be run on all objects issued by the pool before returning them to callers
  /// </param>
  /// <param name="final">
  /// The finalizer that will be run on objects as they return to the pool
  /// </param>
  ObjectPool(
    const autowiring::placement_t&,
    const std::function<void(T*)>& placement = &DefaultPlacement<T>,
    const std::function<void(T&)>& initial = &DefaultInitialize<T>,
    const std::function<void(T&)>& final = &DefaultFinalize<T>
  ) :
    m_monitor(std::make_shared<autowiring::ObjectPoolMonitorT<T>>(this, initial, final)),
    m_placement(placement)
  {}

  ~ObjectPool(void) {
    if(!m_monitor)
      // Nothing to do, type was moved
      return;

    // Transition the pool to the abandoned state:
    m_monitor->Abandon();

    // Clear everything in the cache to ensure that the outstanding limit is correctly updated
    ClearCachedEntities();
  }

protected:
  std::shared_ptr<autowiring::ObjectPoolMonitorT<T>> m_monitor;
  std::condition_variable m_setCondition;

  struct PoolEntry {
    PoolEntry(ObjectPool& pool, size_t poolVersion, const std::function<void(T*)>& placement) :
      monitor(pool.m_monitor),
      poolVersion(poolVersion)
    {
      placement(reinterpret_cast<T*>(obj));
    }

    ~PoolEntry(void) {
      reinterpret_cast<T*>(obj)->~T();
    }

    /// <summary>
    /// Returns this pool entry to the parent monitor object
    /// </summary>
    bool Return(void) {
      // Finalize object before destruction or return to pool.
      monitor->fnl(*reinterpret_cast<T*>(obj));

      // Obtain lock before deciding whether to delete or return to pool.
      std::lock_guard<std::mutex> lk(*monitor);

      // Attempt to return object to pool, allow the caller to handle failure
      return
        monitor->IsAbandoned() ?
        false :
        monitor->owner->ReturnUnsafe(this);
    }

    // Pointer to the monitor used to get us back to our pool when we're returned
    const std::shared_ptr<autowiring::ObjectPoolMonitorT<T>> monitor;

    // Pool version at the time of construction
    const size_t poolVersion;

    // The requested object itself.
    uint8_t obj[sizeof(T)];
  };

  // The set of pooled objects, and the pool version.  The pool version is incremented every
  // time the ClearCachedEntities method is called, and causes entities which might be trying
  // to return to the pool to instead free themselves.
  // IMPORTANT: m_objs cannot be a vector of std::unique_ptr instances because the required move
  // implementation of std::vector is missing when not building with c++11.
  size_t m_poolVersion = 0;
  std::vector<PoolEntry*> m_objs;

  size_t m_maxPooled = ~0;
  size_t m_limit = ~0;
  size_t m_outstanding = 0;

  // Allocator, placement ctor:
  std::function<void(T*)> m_placement{
    [](T* ptr) {
      new (ptr) T;
    }
  };

  /// <summary>
  /// Creates a shared pointer to wrap the specified object while it is issued.
  /// </summary>
  /// <remarks>
  /// The Initialize is applied immediate when Wrap is called.
  /// The Finalize function will be applied is in the shared_ptr destructor.
  /// </remarks>
  std::shared_ptr<T> Wrap(PoolEntry* entry) {
    // Create the shared pointer which will delegate cleanup
    std::shared_ptr<PoolEntry> pe(
      entry,
      [] (PoolEntry* entry) {
        if(!entry->Return())
          delete entry;
      }
    );

    // Initialize the issued object, now that a shared pointer has been created for it.
    m_monitor->initial(*reinterpret_cast<T*>(entry->obj));

    // All done, use an aliased shared pointer here
    return std::shared_ptr<T>(std::move(pe), reinterpret_cast<T*>(entry->obj));
  }

  bool ReturnUnsafe(PoolEntry* ptr) {
    // ASSERT: Object has already been finalized.
    // Always decrement the count when an object is no longer outstanding.
    assert(m_outstanding);
    m_outstanding--;

    bool inPool = false;
    if(
      // Pool versions have to match, or the object should be dumped.
      ptr->poolVersion == m_poolVersion &&

      // Object pool needs to be capable of accepting another object as an input.
      m_objs.size() < m_maxPooled
    ) {
      // Return the object to the pool:
      m_objs.push_back(ptr);
      inPool = true;
    }

    // If the new outstanding count is less than or equal to the limit, wake up any waiters:
    if(m_outstanding <= m_limit)
      m_setCondition.notify_all();

    return inPool;
  }

  /// <summary>
  /// Obtains an element from the object queue, assumes exterior synchronization.
  /// </summary>
  /// <remarks>
  /// This method will unconditionally increment the outstanding count and will not attempt
  /// to perform bounds checking to ensure that the desired element may be issued.
  /// </remarks>
  std::shared_ptr<T> ObtainElementUnsafe(std::unique_lock<std::mutex>& lk) {
    // Unconditionally increment the outstanding count:
    m_outstanding++;

    // Cached, or construct?
    if(m_objs.empty()) {
      // Get the version before releasing the lock, we need to remain coherent with m_outstanding
      size_t poolVersion = m_poolVersion;

      // Lock release, so construction does not have to be synchronized:
      lk.unlock();

      // We failed to recover an object, create a new one:
      return Wrap(
        new PoolEntry(*this, poolVersion, m_placement)
      );
    }

    // Transition from pooled to issued:
    std::shared_ptr<T> iObj = Wrap(m_objs.back());
    m_objs.pop_back();
    return iObj;
  }

public:
  // Accessor methods:
  size_t GetOutstanding(void) const { return m_outstanding; }
  size_t GetCached(void) const {
    std::lock_guard<std::mutex> lk(*m_monitor);
    return m_objs.size();
  }

  bool IsEmpty(void) const {
    std::lock_guard<std::mutex> lk(*m_monitor);
    return m_objs.empty() && !m_outstanding;
  }

  /// <summary>
  /// Discards all entities currently saved in the pool.
  /// </summary>
  /// <remarks>
  /// This method will also cause currently outstanding entities to be freed.  Eventually, once all objects
  /// return to the pool, the set of objects managed by this pool will be distinct from those objects created
  /// prior to this call.
  /// </remarks>
  void ClearCachedEntities(void) {
    std::lock_guard<std::mutex> lk(*m_monitor);
    for (PoolEntry* obj : m_objs)
      delete obj;
    m_objs.clear();
    m_poolVersion++;
  }

  /// <summary>
  /// This sets the maximum number of entities that the pool will cache to satisfy a later allocation request.
  /// </summary>
  /// <param name="maxPooled">The new maximum cache count</param>
  /// <remarks>
  /// If the value of maxPooled is greater than the maximum outstanding limit, it will be made
  /// equal to the maximum outstanding limit.
  /// </remarks>
  void SetMaximumPooledEntities(size_t maxPooled) {
    m_maxPooled = maxPooled;
    for(;;) {
      std::lock_guard<std::mutex> lk(*m_monitor);

      // Space check:
      if(m_objs.size() <= m_maxPooled)
        // Managed to get the size down sufficiently, we can continue:
        return;

      // Remove unique pointer
      delete m_objs.back();
      m_objs.pop_back();
    }
  }

  /// <summary>
  /// Sets the maximum number of objects this pool will permit to be outstanding at time.
  /// </summary>
  /// <remarks>
  /// A user may assign the limit to a value lower than the current limit.  In this case, Wait will block
  /// until the number of outstanding entities falls below the current count.
  ///
  /// If the limit is set to zero, it may not be changed.  Attempting to change the limit in this case
  /// will result in an exception.  Setting the outstanding limit to zero is guaranteed to never throw
  /// an exception.
  /// </remarks>
  void SetOutstandingLimit(size_t limit) {
    std::lock_guard<std::mutex> lk(*m_monitor);
    if(!m_limit && limit)
      // We're throwing an exception if the limit is currently zero and the user is trying to set it
      // to something other than zero.
      throw autowiring_error("Attempted to set the limit to a nonzero value after it was set to zero");
    m_limit = limit;
  }

  /// <summary>
  /// Blocks until an object becomes available from the pool, or the timeout has elapsed.
  /// </summary>
  /// <remarks>
  /// This method will throw an autowiring_error if an attempt is made to obtain an element from a pool
  /// with a limit of zero. If WaitFor returns due to timeout, it will return an invalid shared pointer.
  /// </remarks>
  template<class Duration>
  std::shared_ptr<T> WaitFor(Duration duration) {
    std::unique_lock<std::mutex> lk(*m_monitor);
    if(!m_limit)
      throw autowiring_error("Attempted to perform a timed wait on a pool that is already in rundown");

    if(m_setCondition.wait_for(
        lk,
        duration,
        [this] { return m_outstanding < m_limit; }
      )
    )
      return ObtainElementUnsafe(lk);
    return std::shared_ptr<T>();
  }

  /// <summary>
  /// Blocks until an object becomes available from the pool.
  /// </summary>
  /// <remarks>
  /// This method will throw an autowiring_error if an attempt is made to obtain an element from a pool
  /// with a limit of zero. This method will never return an invalid shared pointer.
  /// </remarks>
  std::shared_ptr<T> Wait(void) {
    std::unique_lock<std::mutex> lk(*m_monitor);
    if(!m_limit)
      throw autowiring_error("Attempted to perform a wait on a pool containing no entities");

    m_setCondition.wait(lk, [this] {
      if(!m_limit)
        throw autowiring_error("Attempted to rundown ObjectPool while performing a wait");

      return m_outstanding < m_limit;
    });
    return ObtainElementUnsafe(lk);
  }

  /// <summary>
  /// Causes the pool's internal cache to hold at least the requested number of items
  /// </summary>
  /// <remarks>
  /// The preallocation routine is an optimization routine similar to vector::reserve.  Calling
  /// this routine can reduce the expense of pointer requests made later, because no allocation
  /// has to take place at that point.
  ///
  /// If the caller requests a reservation that is greater than the maximum pool limit, the
  /// number of objects allocated will be equal to the maximum pool limit.
  /// </remarks>
  void Preallocate(size_t reservation) {
    if(reservation > m_maxPooled)
      reservation = m_maxPooled;

    // Check out objects into our vector, allowing them to all return to the pool at once.
    // This will populate the pool with the desired number of entities, and will trigger
    // the desired failure condition if we wind up checking out more objects than are
    // allowed for this pool.
    std::vector<std::shared_ptr<T>> objs(reservation);
    while(reservation--)
      (*this)(objs[reservation]);
  }

  /// <summary>
  /// Creates a new instance of type T and places it in the passed shared pointer.
  /// </summary>
  /// <remarks>
  /// If the outstanding count is set to anything except -1, this method could potentially
  /// fail and the passed shared_ptr will have a null value.  Callers who use a nonnegative
  /// outstanding limit should be careful to check the return of this function.
  /// </remarks>
  void operator()(std::shared_ptr<T>& rs) {
    // Force the passed value to be empty so we don't return an empty element by accident.
    // This can possibly happen if the pool cap has been reached and we're reobtaining a shared
    // pointer from its own pool.
    rs.reset();

    // Now assign:
    rs = (*this)();
  }

  /// <summary>
  /// Convenience overload of operator().
  /// </summary>
  std::shared_ptr<T> operator()() {
    std::unique_lock<std::mutex> lk(*m_monitor);
    return
      m_limit <= m_outstanding ?

      // Already at the limit
      std::shared_ptr<T>() :

      // Can still check out items at this point
      ObtainElementUnsafe(lk);
  }

  /// <summary>
  /// Blocks until all outstanding entries have been returned, and prevents the issuance of any new items.
  /// </summary>
  /// <remarks>
  /// This method is idempotent.
  /// </remarks>
  void Rundown(void) {
    // Clear our pool and prevent the issuance of any new entities:
    SetMaximumPooledEntities(0);
    SetOutstandingLimit(0);

    // Now, simply block until everyone comes back to us
    std::unique_lock<std::mutex> lk(*m_monitor);
    m_setCondition.wait(lk, [this] {
      return !m_outstanding;
    });
  }

  // Operator overloads
  void operator=(ObjectPool<T>&) = delete;

  void operator=(ObjectPool<T>&& rhs) {
    // Abandon current monitor, we are orphaning current objects
    if(m_monitor) {
      m_monitor->owner = &rhs;
      m_monitor->Abandon();
    }

    // Lock down rhs while we move things in
    std::lock_guard<std::mutex> lk(*rhs.m_monitor);
    std::swap(m_monitor, rhs.m_monitor);

    m_poolVersion = rhs.m_poolVersion;
    m_maxPooled = rhs.m_maxPooled;
    m_limit = rhs.m_limit;
    m_outstanding = rhs.m_outstanding;
    std::swap(m_objs, rhs.m_objs);
    std::swap(m_placement, rhs.m_placement);

    // Now we can take ownership of this monitor object:
    m_monitor->owner = this;
  }
};
