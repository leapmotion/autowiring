//
// detail/select_interrupter.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_SELECT_INTERRUPTER_HPP
#define AUTOBOOST_ASIO_DETAIL_SELECT_INTERRUPTER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if !defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)

#if defined(AUTOBOOST_ASIO_WINDOWS) || defined(__CYGWIN__) || defined(__SYMBIAN32__)
# include <autoboost/asio/detail/socket_select_interrupter.hpp>
#elif defined(AUTOBOOST_ASIO_HAS_EVENTFD)
# include <autoboost/asio/detail/eventfd_select_interrupter.hpp>
#else
# include <autoboost/asio/detail/pipe_select_interrupter.hpp>
#endif

namespace autoboost {
namespace asio {
namespace detail {

#if defined(AUTOBOOST_ASIO_WINDOWS) || defined(__CYGWIN__) || defined(__SYMBIAN32__)
typedef socket_select_interrupter select_interrupter;
#elif defined(AUTOBOOST_ASIO_HAS_EVENTFD)
typedef eventfd_select_interrupter select_interrupter;
#else
typedef pipe_select_interrupter select_interrupter;
#endif

} // namespace detail
} // namespace asio
} // namespace autoboost

#endif // !defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)

#endif // AUTOBOOST_ASIO_DETAIL_SELECT_INTERRUPTER_HPP
