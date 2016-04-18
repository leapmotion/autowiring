//
// detail/win_static_mutex.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_WIN_STATIC_MUTEX_HPP
#define AUTOBOOST_ASIO_DETAIL_WIN_STATIC_MUTEX_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_WINDOWS)

#include <autoboost/asio/detail/scoped_lock.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace detail {

struct win_static_mutex
{
  typedef autoboost::asio::detail::scoped_lock<win_static_mutex> scoped_lock;

  // Initialise the mutex.
  AUTOBOOST_ASIO_DECL void init();

  // Initialisation must be performed in a separate function to the "public"
  // init() function since the compiler does not support the use of structured
  // exceptions and C++ exceptions in the same function.
  AUTOBOOST_ASIO_DECL int do_init();

  // Lock the mutex.
  void lock()
  {
    ::EnterCriticalSection(&crit_section_);
  }

  // Unlock the mutex.
  void unlock()
  {
    ::LeaveCriticalSection(&crit_section_);
  }

  bool initialised_;
  ::CRITICAL_SECTION crit_section_;
};

#if defined(UNDER_CE)
# define AUTOBOOST_ASIO_WIN_STATIC_MUTEX_INIT { false, { 0, 0, 0, 0, 0 } }
#else // defined(UNDER_CE)
# define AUTOBOOST_ASIO_WIN_STATIC_MUTEX_INIT { false, { 0, 0, 0, 0, 0, 0 } }
#endif // defined(UNDER_CE)

} // namespace detail
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#if defined(AUTOBOOST_ASIO_HEADER_ONLY)
# include <autoboost/asio/detail/impl/win_static_mutex.ipp>
#endif // defined(AUTOBOOST_ASIO_HEADER_ONLY)

#endif // defined(AUTOBOOST_ASIO_WINDOWS)

#endif // AUTOBOOST_ASIO_DETAIL_WIN_STATIC_MUTEX_HPP
