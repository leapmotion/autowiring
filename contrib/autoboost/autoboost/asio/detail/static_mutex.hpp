//
// detail/static_mutex.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_STATIC_MUTEX_HPP
#define AUTOBOOST_ASIO_DETAIL_STATIC_MUTEX_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if !defined(AUTOBOOST_ASIO_HAS_THREADS)
# include <autoboost/asio/detail/null_static_mutex.hpp>
#elif defined(AUTOBOOST_ASIO_WINDOWS)
# include <autoboost/asio/detail/win_static_mutex.hpp>
#elif defined(AUTOBOOST_ASIO_HAS_PTHREADS)
# include <autoboost/asio/detail/posix_static_mutex.hpp>
#elif defined(AUTOBOOST_ASIO_HAS_STD_MUTEX_AND_CONDVAR)
# include <autoboost/asio/detail/std_static_mutex.hpp>
#else
# error Only Windows and POSIX are supported!
#endif

namespace autoboost {
namespace asio {
namespace detail {

#if !defined(AUTOBOOST_ASIO_HAS_THREADS)
typedef null_static_mutex static_mutex;
# define AUTOBOOST_ASIO_STATIC_MUTEX_INIT AUTOBOOST_ASIO_NULL_STATIC_MUTEX_INIT
#elif defined(AUTOBOOST_ASIO_WINDOWS)
typedef win_static_mutex static_mutex;
# define AUTOBOOST_ASIO_STATIC_MUTEX_INIT AUTOBOOST_ASIO_WIN_STATIC_MUTEX_INIT
#elif defined(AUTOBOOST_ASIO_HAS_PTHREADS)
typedef posix_static_mutex static_mutex;
# define AUTOBOOST_ASIO_STATIC_MUTEX_INIT AUTOBOOST_ASIO_POSIX_STATIC_MUTEX_INIT
#elif defined(AUTOBOOST_ASIO_HAS_STD_MUTEX_AND_CONDVAR)
typedef std_static_mutex static_mutex;
# define AUTOBOOST_ASIO_STATIC_MUTEX_INIT AUTOBOOST_ASIO_STD_STATIC_MUTEX_INIT
#endif

} // namespace detail
} // namespace asio
} // namespace autoboost

#endif // AUTOBOOST_ASIO_DETAIL_STATIC_MUTEX_HPP
