// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2007 Anthony Williams
// (C) Copyright 2011-2012 Vicente J. Botet Escriba

#ifndef AUTOBOOST_THREAD_LOCK_GUARD_HPP
#define AUTOBOOST_THREAD_LOCK_GUARD_HPP

#include <autoboost/thread/detail/config.hpp>
#include <autoboost/thread/detail/delete.hpp>
#include <autoboost/thread/detail/move.hpp>
#include <autoboost/thread/detail/lockable_wrapper.hpp>
#include <autoboost/thread/lock_options.hpp>
#if ! defined AUTOBOOST_THREAD_PROVIDES_NESTED_LOCKS
#include <autoboost/thread/is_locked_by_this_thread.hpp>
#include <autoboost/assert.hpp>
#endif

#include <autoboost/config/abi_prefix.hpp>

namespace autoboost
{

  template <typename Mutex>
  class lock_guard
  {
  private:
    Mutex& m;

  public:
    typedef Mutex mutex_type;
    AUTOBOOST_THREAD_NO_COPYABLE( lock_guard )

    explicit lock_guard(Mutex& m_) :
      m(m_)
    {
      m.lock();
    }

    lock_guard(Mutex& m_, adopt_lock_t) :
      m(m_)
    {
#if ! defined AUTOBOOST_THREAD_PROVIDES_NESTED_LOCKS
      AUTOBOOST_ASSERT(is_locked_by_this_thread(m));
#endif
    }

#if ! defined AUTOBOOST_THREAD_NO_CXX11_HDR_INITIALIZER_LIST
    lock_guard(std::initializer_list<thread_detail::lockable_wrapper<Mutex> > l_) :
      m(*(const_cast<thread_detail::lockable_wrapper<Mutex>*>(l_.begin())->m))
    {
      m.lock();
    }

    lock_guard(std::initializer_list<thread_detail::lockable_adopt_wrapper<Mutex> > l_) :
      m(*(const_cast<thread_detail::lockable_adopt_wrapper<Mutex>*>(l_.begin())->m))
    {
#if ! defined AUTOBOOST_THREAD_PROVIDES_NESTED_LOCKS
      AUTOBOOST_ASSERT(is_locked_by_this_thread(m));
#endif
    }

#endif
    ~lock_guard()
    {
      m.unlock();
    }
  };


#if ! defined AUTOBOOST_THREAD_NO_MAKE_LOCK_GUARD
  template <typename Lockable>
  lock_guard<Lockable> make_lock_guard(Lockable& mtx)
  {
    return { thread_detail::lockable_wrapper<Lockable>(mtx) };
  }
  template <typename Lockable>
  lock_guard<Lockable> make_lock_guard(Lockable& mtx, adopt_lock_t)
  {
    return { thread_detail::lockable_adopt_wrapper<Lockable>(mtx) };
  }
#endif
}

#include <autoboost/config/abi_suffix.hpp>

#endif
