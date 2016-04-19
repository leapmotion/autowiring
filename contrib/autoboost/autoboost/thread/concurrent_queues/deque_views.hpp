#ifndef AUTOBOOST_THREAD_QUEUE_VIEWS_HPP
#define AUTOBOOST_THREAD_QUEUE_VIEWS_HPP

//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2014. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/thread for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#include <autoboost/thread/detail/config.hpp>
#include <autoboost/thread/detail/move.hpp>
#include <autoboost/thread/concurrent_queues/queue_op_status.hpp>
#include <autoboost/thread/concurrent_queues/deque_base.hpp>

#include <autoboost/config/abi_prefix.hpp>

namespace autoboost
{
namespace concurrent
{

  template <typename Queue>
  class deque_back_view
  {
   Queue* queue;
  public:
    typedef typename Queue::value_type value_type;
    typedef typename Queue::size_type size_type;

    // Constructors/Assignment/Destructors
    deque_back_view(Queue& q) AUTOBOOST_NOEXCEPT : queue(&q) {}

    // Observers
    bool empty() const  { return queue->empty(); }
    bool full() const { return queue->full(); }
    size_type size() const { return queue->size(); }
    bool closed() const { return queue->closed(); }

    // Modifiers
    void close() { queue->close(); }

    void push(const value_type& x) { queue->push_back(x); }

    void pull(value_type& x) { queue->pull_back(x); }
    // enable_if is_nothrow_copy_movable<value_type>
    value_type pull()  { return queue->pull_back(); }

    queue_op_status try_push(const value_type& x) { return queue->try_push_back(x); }

    queue_op_status try_pull(value_type& x) { return queue->try_pull_back(x); }

    queue_op_status nonblocking_push(const value_type& x) { return queue->nonblocking_push_back(x); }

    queue_op_status nonblocking_pull(value_type& x) { return queue->nonblocking_pull_back(x); }

    queue_op_status wait_push(const value_type& x) { return queue->wait_push_back(x); }
    queue_op_status wait_pull(value_type& x) { return queue->wait_pull_back(x); }

    void push(AUTOBOOST_THREAD_RV_REF(value_type) x) { queue->push_back(autoboost::move(x)); }
    queue_op_status try_push(AUTOBOOST_THREAD_RV_REF(value_type) x) { return queue->try_push_back(autoboost::move(x)); }
    queue_op_status nonblocking_push(AUTOBOOST_THREAD_RV_REF(value_type) x) { return queue->nonblocking_push_back(autoboost::move(x)); }
    queue_op_status wait_push(AUTOBOOST_THREAD_RV_REF(value_type) x) { return queue->wait_push_back(autoboost::move(x)); }
  };

  template <typename Queue>
  class deque_front_view
  {
   Queue* queue;
  public:
    typedef typename Queue::value_type value_type;
    typedef typename Queue::size_type size_type;

    // Constructors/Assignment/Destructors
    deque_front_view(Queue& q) AUTOBOOST_NOEXCEPT : queue(&q) {}

    // Observers
    bool empty() const  { return queue->empty(); }
    bool full() const { return queue->full(); }
    size_type size() const { return queue->size(); }
    bool closed() const { return queue->closed(); }

    // Modifiers
    void close() { queue->close(); }

    void push(const value_type& x) { queue->push_front(x); }

    void pull(value_type& x) { queue->pull_front(x); };
    // enable_if is_nothrow_copy_movable<value_type>
    value_type pull()  { return queue->pull_front(); }

    queue_op_status try_push(const value_type& x) { return queue->try_push_front(x); }

    queue_op_status try_pull(value_type& x) { return queue->try_pull_front(x); }

    queue_op_status nonblocking_push(const value_type& x) { return queue->nonblocking_push_front(x); }

    queue_op_status nonblocking_pull(value_type& x) { return queue->nonblocking_pull_front(x); }

    queue_op_status wait_push(const value_type& x) { return queue->wait_push_front(x); }
    queue_op_status wait_pull(value_type& x) { return queue->wait_pull_front(x); }
    void push(AUTOBOOST_THREAD_RV_REF(value_type) x) { queue->push_front(forward<value_type>(x)); }
    queue_op_status try_push(AUTOBOOST_THREAD_RV_REF(value_type) x) { return queue->try_push_front(forward<value_type>(x)); }
    queue_op_status nonblocking_push(AUTOBOOST_THREAD_RV_REF(value_type) x) { return queue->nonblocking_push_front(forward<value_type>(x)); }
    queue_op_status wait_push(AUTOBOOST_THREAD_RV_REF(value_type) x) { return queue->wait_push_front(forward<value_type>(x)); }

  };

#if ! defined AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES

  template <class T>
  using deque_back = deque_back_view<deque_base<T> > ;
  template <class T>
  using deque_front = deque_front_view<deque_base<T> > ;

#else

  template <class T>
  struct deque_back : deque_back_view<deque_base<T> >
  {
    typedef deque_back_view<deque_base<T> > base_type;
    deque_back(deque_base<T>& q) AUTOBOOST_NOEXCEPT : base_type(q) {}
  };
  template <class T>
  struct deque_front : deque_front_view<deque_base<T> >
  {
    typedef deque_front_view<deque_base<T> > base_type;
    deque_front(deque_base<T>& q) AUTOBOOST_NOEXCEPT : base_type(q) {}

  };

#endif

//  template <class Queue>
//  deque_back_view<Queue> back(Queue & q) { return deque_back_view<Queue>(q); }
//  template <class Queue>
//  deque_front_view<Queue> front(Queue & q) { return deque_front_view<Queue>(q); }
//#if 0
//  template <class T>
//  deque_back<T> back(deque_base<T> & q) { return deque_back<T>(q); }
//  template <class T>
//  deque_front<T> front(deque_base<T> & q) { return deque_front<T>(q); }
//#else
//  template <class T>
//  typename deque_back<T>::type back(deque_base<T> & q) { return typename deque_back<T>::type(q); }
//  template <class T>
//  typename deque_front<T>::type front(deque_base<T> & q) { return typename deque_front<T>::type(q); }
//#endif
}

using concurrent::deque_back_view;
using concurrent::deque_front_view;
using concurrent::deque_back;
using concurrent::deque_front;
//using concurrent::back;
//using concurrent::front;

}

#include <autoboost/config/abi_suffix.hpp>

#endif
