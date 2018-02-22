//
// steady_timer.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_STEADY_TIMER_HPP
#define AUTOBOOST_ASIO_STEADY_TIMER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_HAS_STD_CHRONO) \
  || defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_CHRONO) \
  || defined(GENERATING_DOCUMENTATION)

#if defined(AUTOBOOST_ASIO_HAS_STD_CHRONO)
# include <chrono>
#elif defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_CHRONO)
# include <autoboost/chrono/system_clocks.hpp>
#endif

#include <autoboost/asio/basic_waitable_timer.hpp>

namespace autoboost {
namespace asio {

#if defined(GENERATING_DOCUMENTATION)
/// Typedef for a timer based on the steady clock.
/**
 * This typedef uses the C++11 @c &lt;chrono&gt; standard library facility, if
 * available. Otherwise, it may use the Boost.Chrono library. To explicitly
 * utilise Boost.Chrono, use the basic_waitable_timer template directly:
 * @code
 * typedef basic_waitable_timer<autoboost::chrono::steady_clock> timer;
 * @endcode
 */
typedef basic_waitable_timer<chrono::steady_clock> steady_timer;
#elif defined(AUTOBOOST_ASIO_HAS_STD_CHRONO)
# if defined(AUTOBOOST_ASIO_HAS_STD_CHRONO_MONOTONIC_CLOCK)
typedef basic_waitable_timer<std::chrono::monotonic_clock> steady_timer;
# else // defined(AUTOBOOST_ASIO_HAS_STD_CHRONO_MONOTONIC_CLOCK)
typedef basic_waitable_timer<std::chrono::steady_clock> steady_timer;
# endif // defined(AUTOBOOST_ASIO_HAS_STD_CHRONO_MONOTONIC_CLOCK)
#elif defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_CHRONO)
typedef basic_waitable_timer<autoboost::chrono::steady_clock> steady_timer;
#endif

} // namespace asio
} // namespace autoboost

#endif // defined(AUTOBOOST_ASIO_HAS_STD_CHRONO)
       //   || defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_CHRONO)
       //   || defined(GENERATING_DOCUMENTATION)

#endif // AUTOBOOST_ASIO_STEADY_TIMER_HPP
