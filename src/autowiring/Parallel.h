// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "auto_id.h"
#include "DispatchQueue.h"
#include <iterator>
#include <unordered_map>
#include <deque>

class CoreContext;

namespace autowiring {

class parallel;

/// <summary>
/// Iterator that acts as a proxy to the outputs of a parallel structure
/// </summary>
template<typename T>
struct parallel_iterator :
  public std::iterator<std::input_iterator_tag, T>
{
public:
  parallel_iterator(parallel& p, const size_t& remaining):
    m_parent(p),
    m_remaining(remaining)
  {}

protected:
  parallel& m_parent;
  const size_t& m_remaining;

public:
  bool operator!=(const parallel_iterator& rhs) {
    return m_remaining != rhs.m_remaining || &m_parent != &rhs.m_parent;
  }

  // Wrap, required to satisfy InputIterator requirements.
  struct wrap {
    wrap(T val) : val(val) {}

    T val;
    T& operator*(void) { return val; }
  };

  // Iterator operaror overloads:
  parallel_iterator operator++(void);
  wrap operator++(int);
  T operator*(void);
};

template<typename T>
class parallel_collection {
public:
  typedef parallel_iterator<T> iterator;

  explicit parallel_collection(iterator begin, iterator end):
    m_begin(begin),
    m_end(end)
  {}

private:
  iterator m_begin;
  iterator m_end;

public:
  const iterator& begin(void) { return m_begin; }
  const iterator& end(void) const { return m_end; }
};

/// <summary>
/// Full specialization for null responses
/// </summary>
/// <remarks>
/// Technically, this isn't even an iterator, but it's provided to allow parallel::begin to work
/// properly with void as the template type.
/// </remarks>
template<>
struct parallel_iterator<void>
{
public:
  parallel_iterator(parallel& p, const size_t& remaining) :
    m_parent(p),
    m_remaining(remaining)
  {}

protected:
  parallel& m_parent;
  const size_t& m_remaining;

public:
  bool operator!=(const parallel_iterator& rhs) {
    return m_remaining != rhs.m_remaining || &m_parent != &rhs.m_parent;
  }

  parallel_iterator operator++(void) {
    this->operator++(0);
    return *this;
  }
  void operator++(int);

  struct unused {};
  unused operator*(void) const { return{}; };
};

// Provides fan-out and gather functionality. Lambda "jobs" can be started using operator+=
// and gathered using the standard container iteration interface using begin and end. Jobs
// are run in the thread pool of the current context
class parallel {
public:
  /// <summary>
  /// Constructs a parallel instance attached to the current context
  /// </summary>
  /// <remarks>
  /// The thread pool will be sized to the value of std::thread::hardware_concurrency()
  /// </remarks>
  parallel(void);

  /// <summary>
  /// Constructs a parallel instance attached to the current context
  /// </summary>
  parallel(size_t concurrency);

  /// <summary>
  /// Constructs a pool with exactly the specified number of threads and owning context
  /// </summary>
  /// <param name="ctxt">The owning context</param>
  /// <param name="concurrency">The number of parallel threads, set to 0 to use the system default</param>
  /// <remarks>
  /// The context's thread pool is not used for this instance.  The context is only used to obtain
  /// a stop signal for termination and cleanup behaviors.
  /// </remarks>
  parallel(CoreContext& ctxt, size_t concurrency);

  /// <summary>
  /// Non-blocking destructor
  /// </summary>
  /// <remarks>
  /// This destructor will cause running threads to eventually conclude in a safe manner, but will
  /// not block for their completion.  The caller is responsible for manually invoking stop and then
  /// barrier if thread termination must be guaranteed.
  /// </remarks>
  ~parallel(void);

  // Add job to be run in the thread pool
  template<typename _Fx>
  typename std::enable_if<
    !std::is_same<void, typename std::result_of<_Fx()>::type>::value
  >::type
  operator+=(_Fx&& fx) {
    using RetType = typename std::remove_cv<decltype(fx())>::type;

    // Increment remain jobs. This is decremented by calls to "Pop"
    (std::lock_guard<std::mutex>)m_block->m_lock, ++m_block->m_outstandingCount;

    m_block->dq += [this, fx] {
      auto result = std::make_shared<RetType>(fx());

      std::lock_guard<std::mutex>{m_block->m_lock},
      m_block->m_queue[auto_id_t<RetType>{}].emplace_back(std::move(result));
      m_block->m_queueUpdated.notify_all();
    };
  }

  // Specialization for jobs that don't return anything
  template<typename _Fx>
  typename std::enable_if<
    std::is_same<void, typename std::result_of<_Fx()>::type>::value
  >::type
  operator+=(_Fx&& fx) {
    // Increment remain jobs. This is decremented by calls to "Pop"
    (std::lock_guard<std::mutex>)m_block->m_lock, ++m_block->m_outstandingCount;

    m_block->dq += [this, fx] {
      fx();

      std::lock_guard<std::mutex>{m_block->m_lock},
      m_block->m_nVoidEntries++;
      m_block->m_queueUpdated.notify_all();
    };
  }

  // Discard the most recent result. Blocks until the next result arives.
  template<typename T>
  void Pop(void) {
    std::unique_lock<std::mutex> lk(m_block->m_lock);
    if (!m_block->m_outstandingCount)
      throw std::out_of_range("No outstanding jobs");

    if (std::is_same<void, T>::value) {
      m_block->m_queueUpdated.wait(lk, [this] { return m_block->m_nVoidEntries != 0; });
      m_block->m_nVoidEntries--;
    } else {
      auto& qu = m_block->m_queue[auto_id_t<T>{}];
      m_block->m_queueUpdated.wait(lk, [&qu] { return !qu.empty(); });
      qu.pop_front();
    }

    --m_block->m_outstandingCount;
  }

  // Get the most result from the most recent job. Blocks until a result arrives
  // if there isn't one already available
  template<typename T>
  T Top(void) {
    std::unique_lock<std::mutex> lk(m_block->m_lock);

    if (m_block->m_queue[auto_id_t<T>{}].empty())
      m_block->m_queueUpdated.wait(lk, [this]{
        return !m_block->m_queue[auto_id_t<T>{}].empty();
      });
    return *static_cast<T*>(m_block->m_queue[auto_id_t<T>{}].front().ptr());
  }

  // Get a collection containing all entries of the specified type
  template<typename T>
  parallel_collection<T> all(void) {
    return parallel_collection<T> { begin<T>(), end<T>() };
  }

  /// <summary>
  /// Blocks until all outstanding work is done
  /// </summary>
  /// <remarks>
  /// If a stop call has been made, this method will also block until all owned threads have quit
  /// </remarks>
  void barrier(void) {
    std::unique_lock<std::mutex> lk(m_block->m_lock);
    m_block->m_queueUpdated.wait(lk, [this] {
      size_t totalReady = m_block->m_nVoidEntries;
      for (auto& entry : m_block->m_queue)
        totalReady += entry.second.size();
      return m_block->m_outstandingCount == totalReady;
    });
  }

  // Get an iterator to the begining of out queue of job results
  template<typename T>
  parallel_iterator<T> begin(void) {
    return{ *this, m_block->m_outstandingCount };
  }

  // Iterator representing no jobs results remaining
  template<typename T>
  parallel_iterator<T> end(void) {
    static const size_t zero = 0;
    return { *this, zero };
  }

  /// <summary>
  /// Cleanup behavior, causes all running threads in the pool to quit
  /// </summary>
  /// <remarks>
  /// This method does not block.  Use parallel::barrier to wait for all working threads to stop.
  /// </remarks>
  void stop(void);

protected:
  // Internal result maintenance types:
  struct StatusBlock {
    std::mutex m_lock;
    std::condition_variable m_queueUpdated;
    std::unordered_map<auto_id, std::deque<AnySharedPointer>> m_queue;

    // Holds true as long as the owner exists; false once the owner has been destroyed
    bool owned = true;

    // Dispatch queue containing input items:
    DispatchQueue dq;

    // For void entries we don't need a queue, we can just keep a general count of "done"
    size_t m_nVoidEntries = 0;

    // Total number of entries currently outstanding:
    size_t m_outstandingCount = 0;
  };

  // Status block is held outside in order to avoid race conditions
  std::shared_ptr<StatusBlock> m_block = std::make_shared<StatusBlock>();

  // Shared pointer to the enclosing context
  std::shared_ptr<CoreContext> m_ctxt;

  // Termination signal registration with our enclosing context:
  registration_t onStopReg;

  // Unsynchronized version of stop
  void stop_unsafe(void);
};

template<typename T>
parallel_iterator<T> parallel_iterator<T>::operator++(void) {
  m_parent.Pop<T>();
  return *this;
}

template<typename T>
typename parallel_iterator<T>::wrap parallel_iterator<T>::operator++(int) {
  wrap retVal = **this;
  m_parent.Pop<T>();
  return retVal;
}

template<typename T>
T parallel_iterator<T>::operator*(void) {
  return m_parent.Top<T>();
}

}//namespace autowiring
