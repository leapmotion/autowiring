//
// deadline_timer.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DEADLINE_TIMER_HPP
#define AUTOBOOST_ASIO_DEADLINE_TIMER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_DATE_TIME) \
  || defined(GENERATING_DOCUMENTATION)

#include <autoboost/asio/detail/socket_types.hpp> // Must come before posix_time.
#include <autoboost/asio/basic_deadline_timer.hpp>

#include <autoboost/date_time/posix_time/posix_time_types.hpp>

namespace autoboost {
namespace asio {

/// Typedef for the typical usage of timer. Uses a UTC clock.
typedef basic_deadline_timer<autoboost::posix_time::ptime> deadline_timer;

} // namespace asio
} // namespace autoboost

#endif // defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_DATE_TIME)
       // || defined(GENERATING_DOCUMENTATION)

#endif // AUTOBOOST_ASIO_DEADLINE_TIMER_HPP
