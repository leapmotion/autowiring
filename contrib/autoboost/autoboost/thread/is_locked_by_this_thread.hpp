// (C) Copyright 2012 Vicente J. Botet Escriba
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef AUTOBOOST_THREAD_IS_LOCKED_BY_THIS_THREAD_HPP
#define AUTOBOOST_THREAD_IS_LOCKED_BY_THIS_THREAD_HPP

#include <autoboost/thread/detail/config.hpp>

#include <autoboost/config/abi_prefix.hpp>

namespace autoboost
{
  template <typename Lockable>
  class testable_mutex;

  /**
   * Overloaded function used to check if the mutex is locked when it is testable and do nothing otherwise.
   *
   * This function is used usually to assert the pre-condition when the function can only be called when the mutex
   * must be locked by the current thread.
   */
  template <typename Lockable>
  bool is_locked_by_this_thread(testable_mutex<Lockable> const& mtx)
  {
    return mtx.is_locked_by_this_thread();
  }
  template <typename Lockable>
  bool is_locked_by_this_thread(Lockable const&)
  {
    return true;
  }
}

#include <autoboost/config/abi_suffix.hpp>

#endif // header
