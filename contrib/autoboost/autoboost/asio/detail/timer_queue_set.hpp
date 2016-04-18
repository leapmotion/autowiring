//
// detail/timer_queue_set.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_TIMER_QUEUE_SET_HPP
#define AUTOBOOST_ASIO_DETAIL_TIMER_QUEUE_SET_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>
#include <autoboost/asio/detail/timer_queue_base.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace detail {

class timer_queue_set
{
public:
  // Constructor.
  AUTOBOOST_ASIO_DECL timer_queue_set();

  // Add a timer queue to the set.
  AUTOBOOST_ASIO_DECL void insert(timer_queue_base* q);

  // Remove a timer queue from the set.
  AUTOBOOST_ASIO_DECL void erase(timer_queue_base* q);

  // Determine whether all queues are empty.
  AUTOBOOST_ASIO_DECL bool all_empty() const;

  // Get the wait duration in milliseconds.
  AUTOBOOST_ASIO_DECL long wait_duration_msec(long max_duration) const;

  // Get the wait duration in microseconds.
  AUTOBOOST_ASIO_DECL long wait_duration_usec(long max_duration) const;

  // Dequeue all ready timers.
  AUTOBOOST_ASIO_DECL void get_ready_timers(op_queue<operation>& ops);

  // Dequeue all timers.
  AUTOBOOST_ASIO_DECL void get_all_timers(op_queue<operation>& ops);

private:
  timer_queue_base* first_;
};

} // namespace detail
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#if defined(AUTOBOOST_ASIO_HEADER_ONLY)
# include <autoboost/asio/detail/impl/timer_queue_set.ipp>
#endif // defined(AUTOBOOST_ASIO_HEADER_ONLY)

#endif // AUTOBOOST_ASIO_DETAIL_TIMER_QUEUE_SET_HPP
