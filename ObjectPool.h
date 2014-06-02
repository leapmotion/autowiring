#pragma once
#include "autowiring_error.h"
#include "Object.h"
#include "ObjectPoolMonitor.h"
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <set>
#include <functional>
#include RVALUE_HEADER
#include MEMORY_HEADER

template<class T>
T* DefaultCreate(void) {
  return new T;
}

template<typename T>
void DefaultReset(T&){}

/// <summary>
/// Allows the management of a pool of objects based on an embedded factory
/// </summary>
/// <param name="T>The type to be pooled</param>
/// <param name="_Rx">A function object which resets instances returned to the pool</param>
/// <remarks>
/// This class is a type of factory that creates an object of type T.  The object pool
/// creates a shared pointer for the consumer to use, and when the last shared pointer
/// for that object is destroyed, the wrapped object is returned to this pool rather than
/// being deleted.  Returned objects may satisfy subsequent requests for construction.
///
/// All object pool methods are thread safe.
///
/// Issued pool members must be released before the pool goes out of scope
/// </remarks>
template<class T>
class ObjectPool
{
public:
  /// <param name="limit">The maximum number of objects this pool will allow to be outstanding at any time</param>
#if defined(__GNUC__) && !defined(__clang__)
  // workaround for gcc 4.6 internal compiler error
  ObjectPool(
    size_t limit,
    size_t maxPooled,
    const std::function<T*()>& alloc,
    const std::function<void(T&)>& rx
  );
#else
  ObjectPool(
    size_t limit = ~0,
    size_t maxPooled = ~0,
    const std::function<T*()>& alloc = &DefaultCreate<T>,
    const std::function<void(T&)>& rx = &DefaultReset<T>
  ) :
    m_monitor(new ObjectPoolMonitor),
    m_limit(limit),
    m_poolVersion(0),
    m_maxPooled(maxPooled),
    m_outstanding(0),
    m_rx(rx),
    m_alloc(alloc)
  {}
#endif
  ~ObjectPool(void) {
    // Transition the pool to the abandoned state:
    m_monitor->Abandon();

    // Clear everything in the cache to ensure that the outstanding limit is correctly updated
    ClearCachedEntities();
  }

protected:
  std::shared_ptr<ObjectPoolMonitor> m_monitor;
  boost::condition_variable m_setCondition;

  // The set of pooled objects, and the pool version.  The pool version is incremented every
  // time the ClearCachedEntities method is called, and causes entities which might be trying
  // to return to the pool to instead free themselves.
  size_t m_poolVersion;
  std::vector<std::shared_ptr<T>> m_objs;

  size_t m_maxPooled;
  size_t m_limit;
  size_t m_outstanding;

  // Resetter:
  std::function<void(T&)> m_rx;

  // Allocator:
  std::function<T*()> m_alloc;

  /// <summary>
  /// Creates a shared pointer to wrap the specified object
  /// </summary>
  std::shared_ptr<T> Wrap(T* pObj) {
    // Fill the shared pointer with the object we created, and ensure that we override
    // the destructor so that the object is returned to the pool when it falls out of
    // scope.
    auto monitor = m_monitor;
    size_t poolVersion = m_poolVersion;

    return std::shared_ptr<T>(
      pObj,
      [this, poolVersion, monitor](T* ptr) {
        this->Return(poolVersion, monitor, ptr);
      }
    );
  }

  void Return(size_t poolVersion, const std::shared_ptr<ObjectPoolMonitor>& monitor, T* ptr) {
    // Default behavior will be to destroy the pointer
    std::unique_ptr<T> unique(ptr);

    // Hold the lock next, in order to ensure that destruction happens after the monitor
    // lock is released.
    boost::lock_guard<boost::mutex> lk(*monitor);

    if(monitor->IsAbandoned())
      // Nothing we can do, monitor object abandoned already, just destroy the object
      return;

    // Always decrement the count when an object is no longer outstanding
    assert(m_outstanding);
    m_outstanding--;

    if(
      // Pool versions have to match, or the object should be dumped
      poolVersion == m_poolVersion &&

      // Object pool needs to be capable of accepting another object as an input
      m_objs.size() < m_maxPooled
    ) {
      // Reset the object and put it back in the pool:
      m_rx(*ptr);
      m_objs.push_back(Wrap(unique.release()));
    }

    // If the new outstanding count is less than or equal to the limit, wake up any waiters:
    if(m_outstanding <= m_limit)
      m_setCondition.notify_all();
  }

  /// <summary>
  /// Obtains an element from the object queue, assumes exterior synchronization
  /// </summary>
  /// <remarks>
  /// This method will unconditionally increment the outstanding count and will not attempt
  /// to perform bounds checking to ensure that the desired element may be issued
  /// </remarks>
  std::shared_ptr<T> ObtainElementUnsafe(boost::unique_lock<boost::mutex>& lk) {
    // Unconditionally increment the outstanding count:
    m_outstanding++;

    // Cached, or construct?
    if(m_objs.empty()) {
      // Lock release, so construction does not have to be synchronized:
      lk.unlock();

      // We failed to recover an object, create a new one:
      return Wrap(m_alloc());
    }

    // Remove, return:
    auto obj = m_objs.back();
    m_objs.pop_back();
    return obj;
  }

public:
  // Accessor methods:
  size_t GetOutstanding(void) const { return m_outstanding; }
  size_t GetCached(void) const {
    boost::lock_guard<boost::mutex> lk(*m_monitor);
    return m_objs.size();
  }

  // Mutator methods:
  void SetAlloc(const std::function<T*()>& alloc) {
    m_alloc = alloc;
  }

  /// <summary>
  /// Discards all entities currently saved in the pool
  /// </summary>
  /// <remarks>
  /// This method will also cause currently outstanding entities to be freed.  Eventually, once all objects
  /// return to the pool, the set of objects managed by this pool will be distinct from those objects created
  /// prior to this call.
  /// </remarks>
  void ClearCachedEntities(void) {
    // Declare this first, so it's freed last:
    std::vector<std::shared_ptr<T>> objs;

    // Move all of our objects into a local variable which we can then free at our leisure.  This allows us to
    // perform destruction outside of the scope of a lock, preventing any deadlocks that might occur inside
    // the shared_ptr cleanup lambda.
    boost::lock_guard<boost::mutex> lk(*m_monitor);
    m_poolVersion++;

    // Swap the cached object collection with an empty one
    std::swap(objs, m_objs);

    // Technically, all of these entities are now outstanding.  Update accordingly.
    m_outstanding += objs.size();
  }

  /// <summary>
  /// This sets the maximum number of entities that the pool will cache to satisfy a later allocation request
  /// </summary>
  /// <param name="maxPooled">The new maximum cache count</param>
  void SetMaximumPooledEntities(size_t maxPooled) {
    m_maxPooled = maxPooled;
    for(;;) {
      std::shared_ptr<T> prior;
      boost::lock_guard<boost::mutex> lk(*m_monitor);

      // Space check:
      if(m_objs.size() <= m_maxPooled)
        // Managed to get the size down sufficiently, we can continue:
        return;

      // Removing an entry from the cache, must increase the outstanding count at this point
      m_outstanding++;

      // Funny syntax needed to ensure destructors run while we aren't holding any locks.  The prior
      // shared_ptr will be reset after the lock is released, guaranteeing the desired ordering.
      prior = m_objs.back();
      m_objs.pop_back();
    }
  }

  /// <summary>
  /// Sets the maximum number of objects this pool will permit to be outstanding at time
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
    boost::lock_guard<boost::mutex> lk(*m_monitor);
    if(!m_limit && limit)
      // We're throwing an exception if the limit is currently zero and the user is trying to set it 
      // to something other than zero.
      throw autowiring_error("Attempted to set the limit to a nonzero value after it was set to zero");
    m_limit = limit;
  }

  /// <summary>
  /// Blocks until an object becomes available from the pool, or the timeout has elapsed
  /// </summary>
  /// <remarks>
  /// This method will throw an autowiring_error if an attempt is made to obtain an element from a pool
  /// with a limit of zero
  /// </remarks>
  template<class Duration>
  std::shared_ptr<T> WaitFor(Duration duration) {
    boost::unique_lock<boost::mutex> lk(*m_monitor);
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
  /// Blocks until an object becomes available from the pool
  /// </summary>
  /// <remarks>
  /// This method will throw an autowiring_error if an attempt is made to obtain an element from a pool
  /// with a limit of zero
  /// </remarks>
  std::shared_ptr<T> Wait(void) {
    boost::unique_lock<boost::mutex> lk(*m_monitor);
    if(!m_limit)
      throw autowiring_error("Attempted to perform a timed wait on a pool containing no entities");

    m_setCondition.wait(lk, [this] {
      return m_outstanding < m_limit;
    });
    return ObtainElementUnsafe(lk);
  }

  /// <summary>
  /// Creates a new instance of type T and places it in the passed shared pointer
  /// </summary>
  /// <remarks>
  /// If the outstanding count is set to anything except -1, this method could potentially
  /// fail and the passed shared_ptr will have a null value.  Callers who use a nonnegative
  /// outstanding limit should be careful to check the return of this function.
  /// </remarks>
  void operator()(std::shared_ptr<T>& rs) {
    // Force the passed value to be empty so we don't cause a deadlock by accident
    rs.reset();

    boost::unique_lock<boost::mutex> lk(*m_monitor);
    if(m_limit <= m_outstanding)
      // Already at the limit
      return;

    rs = ObtainElementUnsafe(lk);
  }

  /// <summary>
  /// Blocks until all outstanding entries have been returned, and prevents the issuance of any new items
  /// </summary>
  /// <remarks>
  /// This method is idempotent
  /// </remarks>
  void Rundown(void) {
    // Clear our pool and prevent the issuance of any new entities:
    SetMaximumPooledEntities(0);
    SetOutstandingLimit(0);

    // Now, simply block until everyone comes back to us
    boost::unique_lock<boost::mutex> lk(*m_monitor);
    m_setCondition.wait(lk, [this] {
      return !m_outstanding;
    });
  }
};

#if defined(__GNUC__) && !defined(__clang__)
template<class T>
ObjectPool<T>::ObjectPool(
  size_t limit = ~0,
  size_t maxPooled = ~0,
  const std::function<T*()>& alloc = &DefaultCreate<T>,
  const std::function<void(T&)>& rx = [] (T&) {}
) :
  m_monitor(new ObjectPoolMonitor),
  m_limit(limit),
  m_poolVersion(0),
  m_maxPooled(maxPooled),
  m_outstanding(0),
  m_rx(rx),
  m_alloc(alloc)
{}
#endif
