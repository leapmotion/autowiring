// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "Autowired.h"

#include <iterator>
#include <unordered_map>
#include <typeindex>
#include <deque>

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
  // Add job to be run in the thread pool
  template<typename _Fx>
  typename std::enable_if<
    !std::is_same<void, typename std::result_of<_Fx()>::type>::value
  >::type
  operator+=(_Fx&& fx) {
    using RetType = typename std::remove_cv<decltype(fx())>::type;

    // Increment remain jobs. This is decremented by calls to "Pop"
    (std::lock_guard<std::mutex>)m_queueMutex, ++m_outstandingCount;

    *m_ctxt += [this, fx] {
      auto result = std::make_shared<RetType>(fx());

      std::lock_guard<std::mutex> lk(m_queueMutex);
      m_queue[typeid(RetType)].emplace_back(std::move(result));
      m_queueUpdated.notify_all();
    };
  }

  // Specialization for jobs that don't return anything
  template<typename _Fx>
  typename std::enable_if<
    std::is_same<void, typename std::result_of<_Fx()>::type>::value
  >::type
  operator+=(_Fx&& fx) {
    // Increment remain jobs. This is decremented by calls to "Pop"
    (std::lock_guard<std::mutex>)m_queueMutex, ++m_outstandingCount;

    *m_ctxt += [this, fx] {
      fx();

      std::lock_guard<std::mutex> lk(m_queueMutex);
      m_nVoidEntries++;
      m_queueUpdated.notify_all();
    };
  }

  // Discard the most recent result. Blocks until the next result arives.
  template<typename T>
  void Pop(void) {
    std::unique_lock<std::mutex> lk(m_queueMutex);
    if (!m_outstandingCount)
      throw std::out_of_range("No outstanding jobs");

    if (std::is_same<void, T>::value) {
      m_queueUpdated.wait(lk, [this] { return m_nVoidEntries != 0; });
      m_nVoidEntries--;
    } else {
      auto& qu = m_queue[typeid(T)];
      m_queueUpdated.wait(lk, [&qu] { return !qu.empty(); });
      qu.pop_front();
    }

    --m_outstandingCount;
  }

  // Get the most result from the most recent job. Blocks until a result arrives
  // if there isn't one already available
  template<typename T>
  T Top(void) {
    std::unique_lock<std::mutex> lk(m_queueMutex);

    if (m_queue[typeid(T)].empty())
      m_queueUpdated.wait(lk, [this]{
        return !m_queue[typeid(T)].empty();
      });
    return *static_cast<T*>(m_queue[typeid(T)].front().ptr());
  }

  // Get a collection containing all entries of the specified type
  template<typename T>
  parallel_collection<T> all(void) {
    return parallel_collection<T> { begin<T>(), end<T>() };
  }

  // Get an iterator to the begining of out queue of job results
  template<typename T>
  parallel_iterator<T> begin(void) {
    if (!m_ctxt->IsRunning()) {
      if (m_ctxt->IsShutdown())
        throw std::runtime_error("Attempted to enumerate members of a context after the context was shut down");
      else
        throw std::runtime_error("Start the context before attempting to enumerate members of an autowiring::parallel collection");
    }
    return{ *this, m_outstandingCount };
  }

  // Iterator representing no jobs results remaining
  template<typename T>
  parallel_iterator<T> end(void) {
    static const size_t zero = 0;
    return { *this, zero };
  }

protected:
  std::mutex m_queueMutex;
  std::condition_variable m_queueUpdated;
  std::unordered_map<std::type_index, std::deque<AnySharedPointer>> m_queue;

  // For void entries we don't need a queue, we can just keep a general count of "done"
  size_t m_nVoidEntries = 0;

  size_t m_outstandingCount = 0;

  AutoCurrentContext m_ctxt;
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
