#pragma once
#include "Object.h"
#include "ObjectPoolMonitor.h"
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <set>
#include RVALUE_HEADER
#include MEMORY_HEADER

template<class T>
struct NoOp {
  void operator()(T& op) const {}
};

template<class T>
struct Create {
  T* operator()() const { return new T(); }
};

/// <summary>
/// Templated base class for implementors who wish to have more direct control over Reset and Allocate
/// </summary>
template<class T>
class ObjectPoolBase
{
public:
  ObjectPoolBase(size_t limit = ~0, size_t maxPooled = ~0) :
    m_monitor(new ObjectPoolMonitor),
    m_limit(limit),
    m_maxPooled(maxPooled),
    m_outstanding(0)
  {}

  ~ObjectPoolBase(void) {
    // Transition the pool to the abandoned state:
    m_monitor->Abandon();

    for(auto q = m_objs.begin(); q != m_objs.end(); q++)
      delete *q;
  }

protected:
  std::shared_ptr<ObjectPoolMonitor> m_monitor;
  boost::condition_variable m_setCondition;

  typedef std::set<T*> t_stType;
  t_stType m_objs;

  size_t m_maxPooled;
  size_t m_limit;
  size_t m_outstanding;

  void Return(std::shared_ptr<ObjectPoolMonitor> monitor, T* ptr) {
    {
      boost::lock_guard<boost::mutex> lk(*monitor);
      if(!monitor->IsAbandoned()) {
        m_outstanding--;
        if(m_objs.size() < m_maxPooled) {
          // Reset, insert, return
          Reset(*ptr);
          m_objs.insert(ptr);
          m_setCondition.notify_one();
          return;
        }
      }
    }

    // Object wasn't added.  Destroy it.
    delete ptr;
  }

  /// <summary>
  /// Actually performs a reset of the passed object
  /// </summary>
  virtual void Reset(T& ptr) = 0;

  /// <summary>
  /// Creates a new instance of type T
  /// </summary>
  virtual T* Allocate(void) const = 0;

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
    T* pObj;
    if(m_objs.size()) {
      // Lock and remove an element at random:
      auto q = m_objs.begin();
      pObj = *q;
      m_objs.erase(q);
    } else {
      // Lock release, so construction does not have to be synchronized:
      lk.unlock();

      // We failed to recover an object, create a new one:
      pObj = Allocate();
    }

    // Fill the shared pointer with the object we created, and ensure that we override
    // the destructor so that the object is returned to the pool when it falls out of
    // scope.
    auto monitor = m_monitor;
    return std::shared_ptr<T>(
      pObj,
      [this, monitor](T* ptr) {
        // Obtain the monitor lock and return ourselves to the collection:
        this->Return(monitor, ptr);
      }
    );
  }

public:
  // Accessor methods:
  size_t GetOutstanding(void) const { return m_outstanding; }

  void ClearCachedEntities(void) {
    boost::lock_guard<boost::mutex> lk(*m_monitor);
    for (auto q = m_objs.begin(); q != m_objs.end(); q++)
      delete *q;
    m_objs.clear();
  }

  /// <summary>
  /// This sets the maximum number of entities that the pool will cache to satisfy a later allocation request
  /// </summary>
  /// <param name="maxPooled">The new maximum cache count</param>
  bool SetMaximumPooledEntities(bool maxPooled) {
    m_maxPooled = maxPooled;
    for(;;) {
      T* ptr;
      {
        boost::lock_guard<boost::mutex> lk(*m_monitor);
        if(m_objs.size() <= m_maxPooled)
          return false;
        typename t_stType::iterator q = m_objs.begin();
        m_objs.erase(q);
      }
      delete *ptr;
    }
    return true;
  }

  /// <summary>
  /// Sets the maximum number of objects this pool will permit to be outstanding at time
  /// </summary>
  /// <remarks>
  /// </remarks>
  void SetOutstandingLimit(size_t limit) {
    m_limit = limit;
  }

  /// <summary>
  /// Blocks until an object becomes available from the pool, or the timeout has elapsed
  /// </summary>
  template<class Duration>
  std::shared_ptr<T> WaitFor(Duration duration) {
    boost::unique_lock<boost::mutex> lk(*m_monitor);
    if(m_setCondition.wait_for(
        lk,
        duration,
        [this]() -> bool { return m_outstanding < m_limit; }
      )
    )
      return ObtainElementUnsafe(lk);
    return std::shared_ptr<T>();
  }

  /// <summary>
  /// Blocks until an object becomes available from the pool
  /// </summary>
  std::shared_ptr<T> Wait(void) {
    boost::unique_lock<boost::mutex> lk(*m_monitor);
    m_setCondition.wait(lk, [this]() -> bool {
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
};

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
template<class T, class _Rx = NoOp<T>, class _Alloc = Create<T>>
class ObjectPool:
  public ObjectPoolBase<T>
{
public:
  /// <param name="limit">The maximum number of objects this pool will allow to be outstanding at any time</param>
  ObjectPool(size_t limit = ~0, size_t maxPooled = ~0, _Rx&& rx = _Rx(), _Alloc&& alloc = _Alloc()):
    ObjectPoolBase<T>(limit, maxPooled),
    m_rx(std::move(rx)),
    m_alloc(std::move(alloc))
  {}

private:
  // Resetter, where relevant:
  _Rx m_rx;

  // Allocator, where relevant:
  _Alloc m_alloc;

protected:
  void Reset(T& obj) override { m_rx(obj); }
  virtual T* Allocate(void) const override { return m_alloc(); }

public:
  void SetRx(_Rx&& rx) { m_rx = std::move(rx); }
  void SetAlloc(_Alloc&& alloc) { m_alloc = std::move(alloc); }
};
