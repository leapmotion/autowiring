//
// detail/impl/posix_mutex.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_IMPL_POSIX_MUTEX_IPP
#define AUTOBOOST_ASIO_DETAIL_IMPL_POSIX_MUTEX_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_HAS_PTHREADS)

#include <autoboost/asio/detail/posix_mutex.hpp>
#include <autoboost/asio/detail/throw_error.hpp>
#include <autoboost/asio/error.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace detail {

posix_mutex::posix_mutex()
{
  int error = ::pthread_mutex_init(&mutex_, 0);
  autoboost::system::error_code ec(error,
      autoboost::asio::error::get_system_category());
  autoboost::asio::detail::throw_error(ec, "mutex");
}

} // namespace detail
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#endif // defined(AUTOBOOST_ASIO_HAS_PTHREADS)

#endif // AUTOBOOST_ASIO_DETAIL_IMPL_POSIX_MUTEX_IPP
