#ifndef AUTOBOOST_THREAD_SHARED_MUTEX_HPP
#define AUTOBOOST_THREAD_SHARED_MUTEX_HPP

//  shared_mutex.hpp
//
//  (C) Copyright 2007 Anthony Williams
//  (C) Copyright 2011-2012 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <autoboost/thread/detail/config.hpp>
#if defined(AUTOBOOST_THREAD_PLATFORM_WIN32)
#if defined(AUTOBOOST_THREAD_PROVIDES_GENERIC_SHARED_MUTEX_ON_WIN)
#include <autoboost/thread/pthread/shared_mutex.hpp>
#else
#include <autoboost/thread/win32/shared_mutex.hpp>
#endif
#elif defined(AUTOBOOST_THREAD_PLATFORM_PTHREAD)
//#include <autoboost/thread/v2/shared_mutex.hpp>
#include <autoboost/thread/pthread/shared_mutex.hpp>
#else
#error "Boost threads unavailable on this platform"
#endif

#include <autoboost/thread/lockable_traits.hpp>

namespace autoboost
{
  typedef shared_mutex shared_timed_mutex;
  namespace sync
  {
#ifdef AUTOBOOST_THREAD_NO_AUTO_DETECT_MUTEX_TYPES
    template<>
    struct is_basic_lockable<shared_mutex>
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = true);
    };
    template<>
    struct is_lockable<shared_mutex>
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = true);
    };
#endif

  }
}

#endif
