#ifndef _OBJECT_POOL_H
#define _OBJECT_POOL_H
#include <boost/thread/mutex.hpp>
#include <memory>
#include <set>

/// <summary>
/// Allows the management of a pool of objects based on an embedded factory
/// </summary>
/// <remarks>
/// This class is a type of factory that creates an object of type T.  The object pool
/// creates a shared pointer for the consumer to use, and when the last shared pointer
/// for that object is destroyed, the wrapped object is returned to this pool rather than
/// being deleted.  Returned objects may satisfy subsequent requests for construction.
///
/// All object pool methods are thread safe.
/// </remarks>
template<class T>
class ObjectPool
{
public:
  /// <param name="limit">The maximum number of objects this pool will allow to be outstanding at any time</param>
  ObjectPool(size_t limit = ~0, size_t maxPooled = ~0):
    m_limit(limit),
    m_maxPooled(maxPooled),
    m_outstanding(0)
  {}

  ~ObjectPool(void) {
    for(auto q = m_objs.begin(); q != m_objs.end(); q++)
      delete *q;
  }

private:
  boost::mutex m_lock;
  typedef std::set<T*> t_stType;
  t_stType m_objs;

  size_t m_maxPooled;
  size_t m_limit;
  size_t m_outstanding;

protected:
  void Return(T* ptr) {
    {
      boost::lock_guard<boost::mutex> lk(m_lock);
      m_outstanding--;
      if(m_objs.size() < m_maxPooled) {
        m_objs.insert(ptr);
        return;
      }
    }

    // Object wasn't added.  Destroy it.
    delete ptr;
  }

public:
  /// <summary>
  /// This sets the maximum number of entities that the pool will cache to satisfy a later allocation request
  /// </summary>
  /// <param name="maxPooled">The new maximum cache count</param>
  bool SetMaximumPooledEntities(bool maxPooled) {
    m_maxPooled = maxPooled;
    for(;;) {
      T* ptr;
      {
        boost::lock_guard<boost::mutex> lk(m_lock);
        if(m_objs.size() <= m_maxPooled)
          return;
        q = m_objs.begin();
        m_objs.erase(q);
      }
      delete *ptr;
    }
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
  /// Creates a new instance of type T and places it in the passed shared pointer
  /// </summary>
  /// <remarks>
  /// If the outstanding count is set to anything except -1, this method could potentially
  /// fail and the passed shared_ptr will have a null value.  Callers who use a nonnegative
  /// outstanding limit should be careful to check the return of this function.
  /// </remarks>
  void operator()(std::shared_ptr<T>& rs) {
    T* pObj = nullptr;

    // Force the passed value to be empty so we don't cause a deadlock by accident
    rs.reset();

    {
      boost::lock_guard<boost::mutex> lk(m_lock);
      if(m_objs.size()) {
        // Lock and remove an element at random:
        typename t_stType::iterator q = m_objs.begin();
        pObj = *q;
        m_objs.erase(q);
      }
    }

    if(!pObj) {
      if(m_limit == m_outstanding)
        // We do not allow an outstanding count
        return;

      // We failed to recover an object, create a new one:
      pObj = new T;
    }

    // Increment the total number of objects outstanding:
    m_outstanding++;

    // Fill the shared pointer with the object we created, and ensure that we override
    // the destructor so that the object is returned to the pool when it falls out of
    // scope.
    rs.reset(
      pObj,
      [this] (T* ptr) {
        this->Return(ptr);
      }
    );
  }
};

#endif