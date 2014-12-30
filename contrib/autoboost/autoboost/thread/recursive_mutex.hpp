#ifndef AUTOBOOST_THREAD_RECURSIVE_MUTEX_HPP
#define AUTOBOOST_THREAD_RECURSIVE_MUTEX_HPP

//  recursive_mutex.hpp
//
//  (C) Copyright 2007 Anthony Williams
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <autoboost/thread/detail/platform.hpp>
#if defined(AUTOBOOST_THREAD_PLATFORM_WIN32)
#include <autoboost/thread/win32/recursive_mutex.hpp>
#elif defined(AUTOBOOST_THREAD_PLATFORM_PTHREAD)
#include <autoboost/thread/pthread/recursive_mutex.hpp>
#else
#error "Boost threads unavailable on this platform"
#endif

#include <autoboost/thread/lockable_traits.hpp>

namespace autoboost
{
  namespace sync
  {

#ifdef AUTOBOOST_THREAD_NO_AUTO_DETECT_MUTEX_TYPES
    template<>
    struct is_basic_lockable<recursive_mutex>
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = true);
    };
    template<>
    struct is_lockable<recursive_mutex>
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = true);
    };
    template<>
    struct is_basic_lockable<recursive_timed_mutex>
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = true);
    };
    template<>
    struct is_lockable<recursive_timed_mutex>
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = true);
    };
#endif

    template<>
    struct is_recursive_mutex_sur_parolle<recursive_mutex>
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = true);
    };
    template<>
    struct is_recursive_mutex_sur_parolle<recursive_timed_mutex>
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = true);
    };

  }
}
#endif
