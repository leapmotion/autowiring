//
// detail/win_mutex.hpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_WIN_MUTEX_HPP
#define AUTOBOOST_ASIO_DETAIL_WIN_MUTEX_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_WINDOWS)

#include <autoboost/asio/detail/noncopyable.hpp>
#include <autoboost/asio/detail/scoped_lock.hpp>
#include <autoboost/asio/detail/socket_types.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace detail {

class win_mutex
  : private noncopyable
{
public:
  typedef autoboost::asio::detail::scoped_lock<win_mutex> scoped_lock;

  // Constructor.
  AUTOBOOST_ASIO_DECL win_mutex();

  // Destructor.
  ~win_mutex()
  {
    ::DeleteCriticalSection(&crit_section_);
  }

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

private:
  // Initialisation must be performed in a separate function to the constructor
  // since the compiler does not support the use of structured exceptions and
  // C++ exceptions in the same function.
  AUTOBOOST_ASIO_DECL int do_init();

  ::CRITICAL_SECTION crit_section_;
};

} // namespace detail
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#if defined(AUTOBOOST_ASIO_HEADER_ONLY)
# include <autoboost/asio/detail/impl/win_mutex.ipp>
#endif // defined(AUTOBOOST_ASIO_HEADER_ONLY)

#endif // defined(AUTOBOOST_ASIO_WINDOWS)

#endif // AUTOBOOST_ASIO_DETAIL_WIN_MUTEX_HPP
