// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2009-2012 Anthony Williams
// (C) Copyright 2012 Vicente J. Botet Escriba

// Based on the Anthony's idea of thread_joiner in CCiA

#ifndef AUTOBOOST_THREAD_THREAD_GUARD_HPP
#define AUTOBOOST_THREAD_THREAD_GUARD_HPP

#include <autoboost/thread/detail/delete.hpp>
#include <autoboost/thread/detail/move.hpp>
#include <autoboost/thread/thread_functors.hpp>

#include <autoboost/config/abi_prefix.hpp>

namespace autoboost
{

  /**
   * Non-copyable RAII scoped thread guard joiner which join the thread if joinable when destroyed.
   */
  template <class CallableThread = join_if_joinable, class Thread=::autoboost::thread>
  class thread_guard
  {
    Thread& t_;
  public:
    AUTOBOOST_THREAD_NO_COPYABLE( thread_guard)

    explicit thread_guard(Thread& t) :
    t_(t)
    {
    }
    ~thread_guard()
    {
      CallableThread on_destructor;

      on_destructor(t_);
    }
  };

}
#include <autoboost/config/abi_suffix.hpp>

#endif
