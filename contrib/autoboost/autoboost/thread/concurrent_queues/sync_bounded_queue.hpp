#ifndef AUTOBOOST_THREAD_CONCURRENT_QUEUES_SYNC_BOUNDED_QUEUE_HPP
#define AUTOBOOST_THREAD_CONCURRENT_QUEUES_SYNC_BOUNDED_QUEUE_HPP

//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2013-2014. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/thread for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#include <autoboost/thread/detail/config.hpp>
#include <autoboost/thread/condition_variable.hpp>
#include <autoboost/thread/mutex.hpp>
#include <autoboost/thread/detail/move.hpp>
#include <autoboost/throw_exception.hpp>
#include <autoboost/thread/concurrent_queues/queue_op_status.hpp>

#ifndef AUTOBOOST_THREAD_QUEUE_DEPRECATE_OLD
#include <autoboost/smart_ptr/shared_ptr.hpp>
#include <autoboost/smart_ptr/make_shared.hpp>
#endif
#include <autoboost/config/abi_prefix.hpp>

namespace autoboost
{
namespace concurrent
{
  template <typename ValueType>
  class sync_bounded_queue
  {
  public:
    typedef ValueType value_type;
    typedef std::size_t size_type;

    // Constructors/Assignment/Destructors
    AUTOBOOST_THREAD_NO_COPYABLE(sync_bounded_queue)
    explicit sync_bounded_queue(size_type max_elems);
    template <typename Range>
    sync_bounded_queue(size_type max_elems, Range range);
    ~sync_bounded_queue();

    // Observers
    inline bool empty() const;
    inline bool full() const;
    inline size_type capacity() const;
    inline size_type size() const;
    inline bool closed() const;

    // Modifiers
    inline void close();

#ifndef AUTOBOOST_THREAD_QUEUE_DEPRECATE_OLD
    inline void push(const value_type& x);
    inline void push(AUTOBOOST_THREAD_RV_REF(value_type) x);
    inline bool try_push(const value_type& x);
    inline bool try_push(AUTOBOOST_THREAD_RV_REF(value_type) x);
    inline bool try_push(no_block_tag, const value_type& x);
    inline bool try_push(no_block_tag, AUTOBOOST_THREAD_RV_REF(value_type) x);
#endif
    inline void push_back(const value_type& x);
    inline void push_back(AUTOBOOST_THREAD_RV_REF(value_type) x);
    inline queue_op_status try_push_back(const value_type& x);
    inline queue_op_status try_push_back(AUTOBOOST_THREAD_RV_REF(value_type) x);
    inline queue_op_status nonblocking_push_back(const value_type& x);
    inline queue_op_status nonblocking_push_back(AUTOBOOST_THREAD_RV_REF(value_type) x);
    inline queue_op_status wait_push_back(const value_type& x);
    inline queue_op_status wait_push_back(AUTOBOOST_THREAD_RV_REF(value_type) x);

    // Observers/Modifiers
#ifndef AUTOBOOST_THREAD_QUEUE_DEPRECATE_OLD
    inline void pull(value_type&);
    // enable_if is_nothrow_copy_movable<value_type>
    inline value_type pull();
    inline shared_ptr<ValueType> ptr_pull();
    inline bool try_pull(value_type&);
    inline bool try_pull(no_block_tag,value_type&);
    inline shared_ptr<ValueType> try_pull();
#endif
    inline void pull_front(value_type&);
    // enable_if is_nothrow_copy_movable<value_type>
    inline value_type pull_front();
    inline queue_op_status try_pull_front(value_type&);
    inline queue_op_status nonblocking_pull_front(value_type&);

    inline queue_op_status wait_pull_front(ValueType& elem);

  private:
    mutable mutex mtx_;
    condition_variable not_empty_;
    condition_variable not_full_;
    size_type waiting_full_;
    size_type waiting_empty_;
    value_type* data_;
    size_type in_;
    size_type out_;
    size_type capacity_;
    bool closed_;

    inline size_type inc(size_type idx) const AUTOBOOST_NOEXCEPT
    {
      return (idx + 1) % capacity_;
    }

    inline bool empty(unique_lock<mutex>& ) const AUTOBOOST_NOEXCEPT
    {
      return in_ == out_;
    }
    inline bool empty(lock_guard<mutex>& ) const AUTOBOOST_NOEXCEPT
    {
      return in_ == out_;
    }
    inline bool full(unique_lock<mutex>& ) const AUTOBOOST_NOEXCEPT
    {
      return (inc(in_) == out_);
    }
    inline bool full(lock_guard<mutex>& ) const AUTOBOOST_NOEXCEPT
    {
      return (inc(in_) == out_);
    }
    inline size_type capacity(lock_guard<mutex>& ) const AUTOBOOST_NOEXCEPT
    {
      return capacity_-1;
    }
    inline size_type size(lock_guard<mutex>& lk) const AUTOBOOST_NOEXCEPT
    {
      if (full(lk)) return capacity(lk);
      return ((out_+capacity(lk)-in_) % capacity(lk));
    }

    inline void throw_if_closed(unique_lock<mutex>&);
    inline bool closed(unique_lock<mutex>&) const;

#ifndef AUTOBOOST_THREAD_QUEUE_DEPRECATE_OLD
    inline bool try_pull(value_type& x, unique_lock<mutex>& lk);
    inline shared_ptr<value_type> try_pull(unique_lock<mutex>& lk);
    inline bool try_push(const value_type& x, unique_lock<mutex>& lk);
    inline bool try_push(AUTOBOOST_THREAD_RV_REF(value_type) x, unique_lock<mutex>& lk);
#endif
    inline queue_op_status try_pull_front(value_type& x, unique_lock<mutex>& lk);
    inline queue_op_status try_push_back(const value_type& x, unique_lock<mutex>& lk);
    inline queue_op_status try_push_back(AUTOBOOST_THREAD_RV_REF(value_type) x, unique_lock<mutex>& lk);

    inline queue_op_status wait_pull_front(value_type& x, unique_lock<mutex>& lk);
    inline queue_op_status wait_push_back(const value_type& x, unique_lock<mutex>& lk);
    inline queue_op_status wait_push_back(AUTOBOOST_THREAD_RV_REF(value_type) x, unique_lock<mutex>& lk);

    inline void wait_until_not_empty(unique_lock<mutex>& lk);
    inline void wait_until_not_empty(unique_lock<mutex>& lk, bool&);
    inline size_type wait_until_not_full(unique_lock<mutex>& lk);
    inline size_type wait_until_not_full(unique_lock<mutex>& lk, bool&);


    inline void notify_not_empty_if_needed(unique_lock<mutex>& lk)
    {
      if (waiting_empty_ > 0)
      {
        --waiting_empty_;
        lk.unlock();
        not_empty_.notify_one();
      }
    }
    inline void notify_not_full_if_needed(unique_lock<mutex>& lk)
    {
      if (waiting_full_ > 0)
      {
        --waiting_full_;
        lk.unlock();
        not_full_.notify_one();
      }
    }

#ifndef AUTOBOOST_THREAD_QUEUE_DEPRECATE_OLD
    inline void pull(value_type& elem, unique_lock<mutex>& lk)
    {
      elem = autoboost::move(data_[out_]);
      out_ = inc(out_);
      notify_not_full_if_needed(lk);
    }
    inline value_type pull(unique_lock<mutex>& lk)
    {
      value_type elem = autoboost::move(data_[out_]);
      out_ = inc(out_);
      notify_not_full_if_needed(lk);
      return autoboost::move(elem);
    }
    inline autoboost::shared_ptr<value_type> ptr_pull(unique_lock<mutex>& lk)
    {
      shared_ptr<value_type> res = make_shared<value_type>(autoboost::move(data_[out_]));
      out_ = inc(out_);
      notify_not_full_if_needed(lk);
      return res;
    }
#endif
    inline void pull_front(value_type& elem, unique_lock<mutex>& lk)
    {
      elem = autoboost::move(data_[out_]);
      out_ = inc(out_);
      notify_not_full_if_needed(lk);
    }
    inline value_type pull_front(unique_lock<mutex>& lk)
    {
      value_type elem = autoboost::move(data_[out_]);
      out_ = inc(out_);
      notify_not_full_if_needed(lk);
      return autoboost::move(elem);
    }

    inline void set_in(size_type in, unique_lock<mutex>& lk)
    {
      in_ = in;
      notify_not_empty_if_needed(lk);
    }

    inline void push_at(const value_type& elem, size_type in_p_1, unique_lock<mutex>& lk)
    {
      data_[in_] = elem;
      set_in(in_p_1, lk);
    }

    inline void push_at(AUTOBOOST_THREAD_RV_REF(value_type) elem, size_type in_p_1, unique_lock<mutex>& lk)
    {
      data_[in_] = autoboost::move(elem);
      set_in(in_p_1, lk);
    }
  };

  template <typename ValueType>
  sync_bounded_queue<ValueType>::sync_bounded_queue(typename sync_bounded_queue<ValueType>::size_type max_elems) :
    waiting_full_(0), waiting_empty_(0), data_(new value_type[max_elems + 1]), in_(0), out_(0), capacity_(max_elems + 1),
        closed_(false)
  {
    AUTOBOOST_ASSERT_MSG(max_elems >= 1, "number of elements must be > 1");
  }

//  template <typename ValueType>
//  template <typename Range>
//  sync_bounded_queue<ValueType>::sync_bounded_queue(size_type max_elems, Range range) :
//    waiting_full_(0), waiting_empty_(0), data_(new value_type[max_elems + 1]), in_(0), out_(0), capacity_(max_elems + 1),
//        closed_(false)
//  {
//    AUTOBOOST_ASSERT_MSG(max_elems >= 1, "number of elements must be > 1");
//    AUTOBOOST_ASSERT_MSG(max_elems == size(range), "number of elements must match range's size");
//    try
//    {
//      typedef typename Range::iterator iterator_t;
//      iterator_t first = autoboost::begin(range);
//      iterator_t end = autoboost::end(range);
//      size_type in = 0;
//      for (iterator_t cur = first; cur != end; ++cur, ++in)
//      {
//        data_[in] = *cur;
//      }
//      set_in(in);
//    }
//    catch (...)
//    {
//      delete[] data_;
//    }
//  }

  template <typename ValueType>
  sync_bounded_queue<ValueType>::~sync_bounded_queue()
  {
    delete[] data_;
  }

  template <typename ValueType>
  void sync_bounded_queue<ValueType>::close()
  {
    {
      lock_guard<mutex> lk(mtx_);
      closed_ = true;
    }
    not_empty_.notify_all();
    not_full_.notify_all();
  }

  template <typename ValueType>
  bool sync_bounded_queue<ValueType>::closed() const
  {
    lock_guard<mutex> lk(mtx_);
    return closed_;
  }
  template <typename ValueType>
  bool sync_bounded_queue<ValueType>::closed(unique_lock<mutex>& ) const
  {
    return closed_;
  }

  template <typename ValueType>
  bool sync_bounded_queue<ValueType>::empty() const
  {
    lock_guard<mutex> lk(mtx_);
    return empty(lk);
  }
  template <typename ValueType>
  bool sync_bounded_queue<ValueType>::full() const
  {
    lock_guard<mutex> lk(mtx_);
    return full(lk);
  }

  template <typename ValueType>
  typename sync_bounded_queue<ValueType>::size_type sync_bounded_queue<ValueType>::capacity() const
  {
    lock_guard<mutex> lk(mtx_);
    return capacity(lk);
  }

  template <typename ValueType>
  typename sync_bounded_queue<ValueType>::size_type sync_bounded_queue<ValueType>::size() const
  {
    lock_guard<mutex> lk(mtx_);
    return size(lk);
  }

#ifndef AUTOBOOST_THREAD_QUEUE_DEPRECATE_OLD
  template <typename ValueType>
  bool sync_bounded_queue<ValueType>::try_pull(ValueType& elem, unique_lock<mutex>& lk)
  {
    if (empty(lk))
    {
      throw_if_closed(lk);
      return false;
    }
    pull(elem, lk);
    return true;
  }
  template <typename ValueType>
  shared_ptr<ValueType> sync_bounded_queue<ValueType>::try_pull(unique_lock<mutex>& lk)
  {
    if (empty(lk))
    {
      throw_if_closed(lk);
      return shared_ptr<ValueType>();
    }
    return ptr_pull(lk);
  }
  template <typename ValueType>
  bool sync_bounded_queue<ValueType>::try_pull(ValueType& elem)
  {
      unique_lock<mutex> lk(mtx_);
      return try_pull(elem, lk);
  }
#endif

  template <typename ValueType>
  queue_op_status sync_bounded_queue<ValueType>::try_pull_front(ValueType& elem, unique_lock<mutex>& lk)
  {
    if (empty(lk))
    {
      if (closed(lk)) return queue_op_status::closed;
      return queue_op_status::empty;
    }
    pull_front(elem, lk);
    return queue_op_status::success;
  }

  template <typename ValueType>
  queue_op_status sync_bounded_queue<ValueType>::try_pull_front(ValueType& elem)
  {
      unique_lock<mutex> lk(mtx_);
      return try_pull_front(elem, lk);
  }

#ifndef AUTOBOOST_THREAD_QUEUE_DEPRECATE_OLD
  template <typename ValueType>
  bool sync_bounded_queue<ValueType>::try_pull(no_block_tag,ValueType& elem)
  {
      unique_lock<mutex> lk(mtx_, try_to_lock);
      if (!lk.owns_lock())
      {
        return false;
      }
      return try_pull(elem, lk);
  }
  template <typename ValueType>
  autoboost::shared_ptr<ValueType> sync_bounded_queue<ValueType>::try_pull()
  {
      unique_lock<mutex> lk(mtx_);
      return try_pull(lk);
  }
#endif

  template <typename ValueType>
  queue_op_status sync_bounded_queue<ValueType>::nonblocking_pull_front(ValueType& elem)
  {
      unique_lock<mutex> lk(mtx_, try_to_lock);
      if (!lk.owns_lock())
      {
        return queue_op_status::busy;
      }
      return try_pull_front(elem, lk);
  }

  template <typename ValueType>
  void sync_bounded_queue<ValueType>::throw_if_closed(unique_lock<mutex>&)
  {
    if (closed_)
    {
      AUTOBOOST_THROW_EXCEPTION( sync_queue_is_closed() );
    }
  }

  template <typename ValueType>
  void sync_bounded_queue<ValueType>::wait_until_not_empty(unique_lock<mutex>& lk)
  {
    for (;;)
    {
      if (out_ != in_) break;
      throw_if_closed(lk);
      ++waiting_empty_;
      not_empty_.wait(lk);
    }
  }
  template <typename ValueType>
  void sync_bounded_queue<ValueType>::wait_until_not_empty(unique_lock<mutex>& lk, bool & closed)
  {
    for (;;)
    {
      if (out_ != in_) break;
      if (closed_) {closed=true; return;}
      ++waiting_empty_;
      not_empty_.wait(lk);
    }
  }

#ifndef AUTOBOOST_THREAD_QUEUE_DEPRECATE_OLD
  template <typename ValueType>
  void sync_bounded_queue<ValueType>::pull(ValueType& elem)
  {
      unique_lock<mutex> lk(mtx_);
      wait_until_not_empty(lk);
      pull(elem, lk);
  }
//  template <typename ValueType>
//  void sync_bounded_queue<ValueType>::pull(ValueType& elem, bool & closed)
//  {
//      unique_lock<mutex> lk(mtx_);
//      wait_until_not_empty(lk, closed);
//      if (closed) {return;}
//      pull(elem, lk);
//  }

  // enable if ValueType is nothrow movable
  template <typename ValueType>
  ValueType sync_bounded_queue<ValueType>::pull()
  {
      unique_lock<mutex> lk(mtx_);
      wait_until_not_empty(lk);
      return pull(lk);
  }
  template <typename ValueType>
  autoboost::shared_ptr<ValueType> sync_bounded_queue<ValueType>::ptr_pull()
  {
      unique_lock<mutex> lk(mtx_);
      wait_until_not_empty(lk);
      return ptr_pull(lk);
  }

#endif

  template <typename ValueType>
  void sync_bounded_queue<ValueType>::pull_front(ValueType& elem)
  {
      unique_lock<mutex> lk(mtx_);
      wait_until_not_empty(lk);
      pull_front(elem, lk);
  }

  // enable if ValueType is nothrow movable
  template <typename ValueType>
  ValueType sync_bounded_queue<ValueType>::pull_front()
  {
      unique_lock<mutex> lk(mtx_);
      wait_until_not_empty(lk);
      return pull_front(lk);
  }

  template <typename ValueType>
  queue_op_status sync_bounded_queue<ValueType>::wait_pull_front(ValueType& elem, unique_lock<mutex>& lk)
  {
      if (empty(lk) && closed(lk)) {return queue_op_status::closed;}
      wait_until_not_empty(lk);
      pull_front(elem, lk);
      return queue_op_status::success;
  }
  template <typename ValueType>
  queue_op_status sync_bounded_queue<ValueType>::wait_pull_front(ValueType& elem)
  {
    unique_lock<mutex> lk(mtx_);
    return wait_pull_front(elem, lk);
  }

#ifndef AUTOBOOST_THREAD_QUEUE_DEPRECATE_OLD
  template <typename ValueType>
  bool sync_bounded_queue<ValueType>::try_push(const ValueType& elem, unique_lock<mutex>& lk)
  {
    throw_if_closed(lk);
    size_type in_p_1 = inc(in_);
    if (in_p_1 == out_)  // full()
    {
      return false;
    }
    push_at(elem, in_p_1, lk);
    return true;
  }
  template <typename ValueType>
  bool sync_bounded_queue<ValueType>::try_push(const ValueType& elem)
  {
      unique_lock<mutex> lk(mtx_);
      return try_push(elem, lk);
  }

#endif

  template <typename ValueType>
  queue_op_status sync_bounded_queue<ValueType>::try_push_back(const ValueType& elem, unique_lock<mutex>& lk)
  {
    if (closed(lk)) return queue_op_status::closed;
    size_type in_p_1 = inc(in_);
    if (in_p_1 == out_)  // full()
    {
      return queue_op_status::full;
    }
    push_at(elem, in_p_1, lk);
    return queue_op_status::success;
  }

  template <typename ValueType>
  queue_op_status sync_bounded_queue<ValueType>::try_push_back(const ValueType& elem)
  {
    unique_lock<mutex> lk(mtx_);
    return try_push_back(elem, lk);
  }

  template <typename ValueType>
  queue_op_status sync_bounded_queue<ValueType>::wait_push_back(const ValueType& elem, unique_lock<mutex>& lk)
  {
    if (closed(lk)) return queue_op_status::closed;
    push_at(elem, wait_until_not_full(lk), lk);
    return queue_op_status::success;
  }
  template <typename ValueType>
  queue_op_status sync_bounded_queue<ValueType>::wait_push_back(const ValueType& elem)
  {
    unique_lock<mutex> lk(mtx_);
    return wait_push_back(elem, lk);
  }


#ifndef AUTOBOOST_THREAD_QUEUE_DEPRECATE_OLD
  template <typename ValueType>
  bool sync_bounded_queue<ValueType>::try_push(no_block_tag, const ValueType& elem)
  {
      unique_lock<mutex> lk(mtx_, try_to_lock);
      if (!lk.owns_lock()) return false;
      return try_push(elem, lk);
  }
#endif

  template <typename ValueType>
  queue_op_status sync_bounded_queue<ValueType>::nonblocking_push_back(const ValueType& elem)
  {
    unique_lock<mutex> lk(mtx_, try_to_lock);
    if (!lk.owns_lock()) return queue_op_status::busy;
    return try_push_back(elem, lk);
  }

  template <typename ValueType>
  typename sync_bounded_queue<ValueType>::size_type sync_bounded_queue<ValueType>::wait_until_not_full(unique_lock<mutex>& lk)
  {
    for (;;)
    {
      throw_if_closed(lk);
      size_type in_p_1 = inc(in_);
      if (in_p_1 != out_) // ! full()
      {
        return in_p_1;
      }
      ++waiting_full_;
      not_full_.wait(lk);
    }
  }

#ifndef AUTOBOOST_THREAD_QUEUE_DEPRECATE_OLD
  template <typename ValueType>
  void sync_bounded_queue<ValueType>::push(const ValueType& elem)
  {
      unique_lock<mutex> lk(mtx_);
      push_at(elem, wait_until_not_full(lk), lk);
  }
#endif
  template <typename ValueType>
  void sync_bounded_queue<ValueType>::push_back(const ValueType& elem)
  {
      unique_lock<mutex> lk(mtx_);
      push_at(elem, wait_until_not_full(lk), lk);
  }

#ifndef AUTOBOOST_THREAD_QUEUE_DEPRECATE_OLD
  template <typename ValueType>
  bool sync_bounded_queue<ValueType>::try_push(AUTOBOOST_THREAD_RV_REF(ValueType) elem, unique_lock<mutex>& lk)
  {
    throw_if_closed(lk);
    size_type in_p_1 = inc(in_);
    if (in_p_1 == out_) // full()
    {
      return false;
    }
    push_at(autoboost::move(elem), in_p_1, lk);
    return true;
  }

  template <typename ValueType>
  bool sync_bounded_queue<ValueType>::try_push(AUTOBOOST_THREAD_RV_REF(ValueType) elem)
  {
      unique_lock<mutex> lk(mtx_);
      return try_push(autoboost::move(elem), lk);
  }
#endif

  template <typename ValueType>
  queue_op_status sync_bounded_queue<ValueType>::try_push_back(AUTOBOOST_THREAD_RV_REF(ValueType) elem, unique_lock<mutex>& lk)
  {
    if (closed(lk)) return queue_op_status::closed;
    size_type in_p_1 = inc(in_);
    if (in_p_1 == out_) // full()
    {
      return queue_op_status::full;
    }
    push_at(autoboost::move(elem), in_p_1, lk);
    return queue_op_status::success;
  }
  template <typename ValueType>
  queue_op_status sync_bounded_queue<ValueType>::try_push_back(AUTOBOOST_THREAD_RV_REF(ValueType) elem)
  {
      unique_lock<mutex> lk(mtx_);
      return try_push_back(autoboost::move(elem), lk);
  }

  template <typename ValueType>
  queue_op_status sync_bounded_queue<ValueType>::wait_push_back(AUTOBOOST_THREAD_RV_REF(ValueType) elem, unique_lock<mutex>& lk)
  {
    if (closed(lk)) return queue_op_status::closed;
    push_at(autoboost::move(elem), wait_until_not_full(lk), lk);
    return queue_op_status::success;
  }
  template <typename ValueType>
  queue_op_status sync_bounded_queue<ValueType>::wait_push_back(AUTOBOOST_THREAD_RV_REF(ValueType) elem)
  {
      unique_lock<mutex> lk(mtx_);
      return try_push_back(autoboost::move(elem), lk);
  }


#ifndef AUTOBOOST_THREAD_QUEUE_DEPRECATE_OLD
  template <typename ValueType>
  bool sync_bounded_queue<ValueType>::try_push(no_block_tag, AUTOBOOST_THREAD_RV_REF(ValueType) elem)
  {
      unique_lock<mutex> lk(mtx_, try_to_lock);
      if (!lk.owns_lock())
      {
        return false;
      }
      return try_push(autoboost::move(elem), lk);
  }
#endif
  template <typename ValueType>
  queue_op_status sync_bounded_queue<ValueType>::nonblocking_push_back(AUTOBOOST_THREAD_RV_REF(ValueType) elem)
  {
      unique_lock<mutex> lk(mtx_, try_to_lock);
      if (!lk.owns_lock())
      {
        return queue_op_status::busy;
      }
      return try_push_back(autoboost::move(elem), lk);
  }

#ifndef AUTOBOOST_THREAD_QUEUE_DEPRECATE_OLD
  template <typename ValueType>
  void sync_bounded_queue<ValueType>::push(AUTOBOOST_THREAD_RV_REF(ValueType) elem)
  {
      unique_lock<mutex> lk(mtx_);
      push_at(autoboost::move(elem), wait_until_not_full(lk), lk);
  }
#endif
  template <typename ValueType>
  void sync_bounded_queue<ValueType>::push_back(AUTOBOOST_THREAD_RV_REF(ValueType) elem)
  {
      unique_lock<mutex> lk(mtx_);
      push_at(autoboost::move(elem), wait_until_not_full(lk), lk);
  }

  template <typename ValueType>
  sync_bounded_queue<ValueType>& operator<<(sync_bounded_queue<ValueType>& sbq, AUTOBOOST_THREAD_RV_REF(ValueType) elem)
  {
    sbq.push_back(autoboost::move(elem));
    return sbq;
  }

  template <typename ValueType>
  sync_bounded_queue<ValueType>& operator<<(sync_bounded_queue<ValueType>& sbq, ValueType const&elem)
  {
    sbq.push_back(elem);
    return sbq;
  }

  template <typename ValueType>
  sync_bounded_queue<ValueType>& operator>>(sync_bounded_queue<ValueType>& sbq, ValueType &elem)
  {
    sbq.pull_front(elem);
    return sbq;
  }
}
using concurrent::sync_bounded_queue;

}

#include <autoboost/config/abi_suffix.hpp>

#endif
