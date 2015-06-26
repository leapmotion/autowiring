// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "Autowired.h"

#include <iterator>
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <deque>

namespace autowiring {

class parallel {
public:

  template<typename _Fx>
  void operator+=(_Fx&& fx) {
    using RetType = typename std::remove_cv<decltype(fx())>::type;
    {
      std::lock_guard<std::mutex> lk(m_queueMutex);
      ++m_outstandingCount;
    }

    m_ctxt += [this, fx] {
      auto result = std::make_shared<RetType>(fx());

      std::lock_guard<std::mutex> lk(m_queueMutex);
      m_queue[typeid(RetType)].emplace_back(std::move(result));
      --m_outstandingCount;
      m_queueUpdated.notify_all();
    };
  }

  template<typename T>
  struct iterator:
    public std::iterator<std::input_iterator_tag, T>
  {
    iterator(parallel* p, size_t& remaining):
      m_parent(p),
      m_remaining(remaining)
    {}

    bool operator==(const iterator& rhs) {
      return m_parent == rhs.m_parent && m_remaining == rhs.m_remaining;
    }

    T operator++(void) {
      std::unique_lock<std::mutex> lk(m_queueMutex);

      if (m_parent->m_queue.empty())
        m_parent->m_queueUpdated.wait(lk, [this]{
          return !m_parent->m_queue[typeid(T)].empty();
        });

      auto& queue = m_queue[typeid(T)];

      T retval = queue.front().template as<T>();
      queue.pop_front();
      m_remaining--;

      return retval;
    }

  protected:
    parallel* m_parent;
    size_t& m_remaining;
  };

  template<typename T>
  iterator<T> begin(void) {
    std::lock_guard<std::mutex> lk(m_queueMutex);

    iterator<T> retval(this, m_outstandingCount);
    return retval;
  }

  template<typename T>
  iterator<T> end(void) {
    return iterator<T>(this, 0);
  }


protected:
  std::mutex m_queueMutex;
  std::condition_variable m_queueUpdated;
  std::unordered_map<std::type_index, std::deque<AnySharedPointer>> m_queue;

  size_t m_outstandingCount = 0;

  AutoCurrentContext m_ctxt;
};




}//namespace autowiring
