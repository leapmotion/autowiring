//
// detail/reactor.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_REACTOR_HPP
#define AUTOBOOST_ASIO_DETAIL_REACTOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/reactor_fwd.hpp>

#if defined(AUTOBOOST_ASIO_HAS_EPOLL)
# include <autoboost/asio/detail/epoll_reactor.hpp>
#elif defined(AUTOBOOST_ASIO_HAS_KQUEUE)
# include <autoboost/asio/detail/kqueue_reactor.hpp>
#elif defined(AUTOBOOST_ASIO_HAS_DEV_POLL)
# include <autoboost/asio/detail/dev_poll_reactor.hpp>
#elif defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)
# include <autoboost/asio/detail/null_reactor.hpp>
#else
# include <autoboost/asio/detail/select_reactor.hpp>
#endif

#endif // AUTOBOOST_ASIO_DETAIL_REACTOR_HPP
