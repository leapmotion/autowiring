// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "Autowired.h"

#include <iterator>
#include <unordered_map>
#include <typeindex>
#include <deque>

namespace autowiring {

// Provides fan-out and gather functionality. Lambda "jobs" can be started using operator+=
// and gathered using the standard container iteration interface using begin and end. Jobs
// are run in the thread pool of the current context
class parallel {
public:

  // Add job to be run in the thread pool
  template<typename _Fx>
  void operator+=(_Fx&& fx) {
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

  // Discard the most recent result. Blocks until the next result arives.
  template<typename T>
  void Pop(void) {
    std::unique_lock<std::mutex> lk(m_queueMutex);

    if (m_queue[typeid(T)].empty())
      if (!m_outstandingCount)
        throw std::out_of_range("No outstanding jobs");

      m_queueUpdated.wait(lk, [this]{
        return !m_queue[typeid(T)].empty();
      });

    m_queue[typeid(T)].pop_front();
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
    return *static_cast<T*>(m_queue[typeid(T)].front()->ptr());
  }

  // Iterator that acts as a proxy to
  template<typename T>
  struct iterator:
    public std::iterator<std::input_iterator_tag, T>
  {
    iterator(parallel& p, const size_t& remaining):
      m_parent(p),
      m_remaining(remaining)
    {}

    bool operator!=(const iterator& rhs) {
      return m_remaining != rhs.m_remaining || &m_parent != &rhs.m_parent;
    }

    iterator<T> operator++(void) {
      m_parent.Pop<T>();
      return *this;
    }

    T operator*(void) {
      return m_parent.Top<T>();
    }

  protected:
    parallel& m_parent;
    const size_t& m_remaining;
  };

  // Get an iterator to the begining of out queue of job results
  template<typename T>
  iterator<T> begin(void) {
    return iterator<T>(*this, m_outstandingCount);
  }

  // Iterator representing no jobs results remaining
  template<typename T>
  iterator<T> end(void) {
    static const size_t empty = 0;
    return iterator<T>(*this, empty);
  }


protected:
  std::mutex m_queueMutex;
  std::condition_variable m_queueUpdated;
  std::unordered_map<std::type_index, std::deque<AnySharedPointer>> m_queue;

  size_t m_outstandingCount = 0;

  AutoCurrentContext m_ctxt;
};


}//namespace autowiring
