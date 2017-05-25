//
// detail/thread.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_THREAD_HPP
#define AUTOBOOST_ASIO_DETAIL_THREAD_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if !defined(AUTOBOOST_ASIO_HAS_THREADS)
# include <autoboost/asio/detail/null_thread.hpp>
#elif defined(AUTOBOOST_ASIO_WINDOWS)
# if defined(AUTOBOOST_ASIO_WINDOWS_APP) || defined(UNDER_CE)
#  include <autoboost/asio/detail/winapi_thread.hpp>
# else
#  include <autoboost/asio/detail/win_thread.hpp>
# endif
#elif defined(AUTOBOOST_ASIO_HAS_PTHREADS)
# include <autoboost/asio/detail/posix_thread.hpp>
#elif defined(AUTOBOOST_ASIO_HAS_STD_THREAD)
# include <autoboost/asio/detail/std_thread.hpp>
#else
# error Only Windows, POSIX and std::thread are supported!
#endif

namespace autoboost {
namespace asio {
namespace detail {

#if !defined(AUTOBOOST_ASIO_HAS_THREADS)
typedef null_thread thread;
#elif defined(AUTOBOOST_ASIO_WINDOWS)
# if defined(AUTOBOOST_ASIO_WINDOWS_APP) || defined(UNDER_CE)
typedef winapi_thread thread;
# else
typedef win_thread thread;
# endif
#elif defined(AUTOBOOST_ASIO_HAS_PTHREADS)
typedef posix_thread thread;
#elif defined(AUTOBOOST_ASIO_HAS_STD_THREAD)
typedef std_thread thread;
#endif

} // namespace detail
} // namespace asio
} // namespace autoboost

#endif // AUTOBOOST_ASIO_DETAIL_THREAD_HPP
