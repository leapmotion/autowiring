//
// detail/select_reactor.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_SELECT_REACTOR_HPP
#define AUTOBOOST_ASIO_DETAIL_SELECT_REACTOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_HAS_IOCP) \
  || (!defined(AUTOBOOST_ASIO_HAS_DEV_POLL) \
      && !defined(AUTOBOOST_ASIO_HAS_EPOLL) \
      && !defined(AUTOBOOST_ASIO_HAS_KQUEUE) \
      && !defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME))

#include <cstddef>
#include <autoboost/asio/detail/fd_set_adapter.hpp>
#include <autoboost/asio/detail/limits.hpp>
#include <autoboost/asio/detail/mutex.hpp>
#include <autoboost/asio/detail/op_queue.hpp>
#include <autoboost/asio/detail/reactor_op.hpp>
#include <autoboost/asio/detail/reactor_op_queue.hpp>
#include <autoboost/asio/detail/select_interrupter.hpp>
#include <autoboost/asio/detail/socket_types.hpp>
#include <autoboost/asio/detail/timer_queue_base.hpp>
#include <autoboost/asio/detail/timer_queue_set.hpp>
#include <autoboost/asio/detail/wait_op.hpp>
#include <autoboost/asio/io_service.hpp>

#if defined(AUTOBOOST_ASIO_HAS_IOCP)
# include <autoboost/asio/detail/thread.hpp>
#endif // defined(AUTOBOOST_ASIO_HAS_IOCP)

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace detail {

class select_reactor
  : public autoboost::asio::detail::service_base<select_reactor>
{
public:
#if defined(AUTOBOOST_ASIO_WINDOWS) || defined(__CYGWIN__)
  enum op_types { read_op = 0, write_op = 1, except_op = 2,
    max_select_ops = 3, connect_op = 3, max_ops = 4 };
#else // defined(AUTOBOOST_ASIO_WINDOWS) || defined(__CYGWIN__)
  enum op_types { read_op = 0, write_op = 1, except_op = 2,
    max_select_ops = 3, connect_op = 1, max_ops = 3 };
#endif // defined(AUTOBOOST_ASIO_WINDOWS) || defined(__CYGWIN__)

  // Per-descriptor data.
  struct per_descriptor_data
  {
  };

  // Constructor.
  AUTOBOOST_ASIO_DECL select_reactor(autoboost::asio::io_service& io_service);

  // Destructor.
  AUTOBOOST_ASIO_DECL ~select_reactor();

  // Destroy all user-defined handler objects owned by the service.
  AUTOBOOST_ASIO_DECL void shutdown_service();

  // Recreate internal descriptors following a fork.
  AUTOBOOST_ASIO_DECL void fork_service(
      autoboost::asio::io_service::fork_event fork_ev);

  // Initialise the task, but only if the reactor is not in its own thread.
  AUTOBOOST_ASIO_DECL void init_task();

  // Register a socket with the reactor. Returns 0 on success, system error
  // code on failure.
  AUTOBOOST_ASIO_DECL int register_descriptor(socket_type, per_descriptor_data&);

  // Register a descriptor with an associated single operation. Returns 0 on
  // success, system error code on failure.
  AUTOBOOST_ASIO_DECL int register_internal_descriptor(
      int op_type, socket_type descriptor,
      per_descriptor_data& descriptor_data, reactor_op* op);

  // Post a reactor operation for immediate completion.
  void post_immediate_completion(reactor_op* op, bool is_continuation)
  {
    io_service_.post_immediate_completion(op, is_continuation);
  }

  // Start a new operation. The reactor operation will be performed when the
  // given descriptor is flagged as ready, or an error has occurred.
  AUTOBOOST_ASIO_DECL void start_op(int op_type, socket_type descriptor,
      per_descriptor_data&, reactor_op* op, bool is_continuation, bool);

  // Cancel all operations associated with the given descriptor. The
  // handlers associated with the descriptor will be invoked with the
  // operation_aborted error.
  AUTOBOOST_ASIO_DECL void cancel_ops(socket_type descriptor, per_descriptor_data&);

  // Cancel any operations that are running against the descriptor and remove
  // its registration from the reactor.
  AUTOBOOST_ASIO_DECL void deregister_descriptor(socket_type descriptor,
      per_descriptor_data&, bool closing);

  // Remote the descriptor's registration from the reactor.
  AUTOBOOST_ASIO_DECL void deregister_internal_descriptor(
      socket_type descriptor, per_descriptor_data& descriptor_data);

  // Move descriptor registration from one descriptor_data object to another.
  AUTOBOOST_ASIO_DECL void move_descriptor(socket_type descriptor,
      per_descriptor_data& target_descriptor_data,
      per_descriptor_data& source_descriptor_data);

  // Add a new timer queue to the reactor.
  template <typename Time_Traits>
  void add_timer_queue(timer_queue<Time_Traits>& queue);

  // Remove a timer queue from the reactor.
  template <typename Time_Traits>
  void remove_timer_queue(timer_queue<Time_Traits>& queue);

  // Schedule a new operation in the given timer queue to expire at the
  // specified absolute time.
  template <typename Time_Traits>
  void schedule_timer(timer_queue<Time_Traits>& queue,
      const typename Time_Traits::time_type& time,
      typename timer_queue<Time_Traits>::per_timer_data& timer, wait_op* op);

  // Cancel the timer operations associated with the given token. Returns the
  // number of operations that have been posted or dispatched.
  template <typename Time_Traits>
  std::size_t cancel_timer(timer_queue<Time_Traits>& queue,
      typename timer_queue<Time_Traits>::per_timer_data& timer,
      std::size_t max_cancelled = (std::numeric_limits<std::size_t>::max)());

  // Run select once until interrupted or events are ready to be dispatched.
  AUTOBOOST_ASIO_DECL void run(bool block, op_queue<operation>& ops);

  // Interrupt the select loop.
  AUTOBOOST_ASIO_DECL void interrupt();

private:
#if defined(AUTOBOOST_ASIO_HAS_IOCP)
  // Run the select loop in the thread.
  AUTOBOOST_ASIO_DECL void run_thread();

  // Entry point for the select loop thread.
  AUTOBOOST_ASIO_DECL static void call_run_thread(select_reactor* reactor);
#endif // defined(AUTOBOOST_ASIO_HAS_IOCP)

  // Helper function to add a new timer queue.
  AUTOBOOST_ASIO_DECL void do_add_timer_queue(timer_queue_base& queue);

  // Helper function to remove a timer queue.
  AUTOBOOST_ASIO_DECL void do_remove_timer_queue(timer_queue_base& queue);

  // Get the timeout value for the select call.
  AUTOBOOST_ASIO_DECL timeval* get_timeout(timeval& tv);

  // Cancel all operations associated with the given descriptor. This function
  // does not acquire the select_reactor's mutex.
  AUTOBOOST_ASIO_DECL void cancel_ops_unlocked(socket_type descriptor,
      const autoboost::system::error_code& ec);

  // The io_service implementation used to post completions.
  io_service_impl& io_service_;

  // Mutex to protect access to internal data.
  autoboost::asio::detail::mutex mutex_;

  // The interrupter is used to break a blocking select call.
  select_interrupter interrupter_;

  // The queues of read, write and except operations.
  reactor_op_queue<socket_type> op_queue_[max_ops];

  // The file descriptor sets to be passed to the select system call.
  fd_set_adapter fd_sets_[max_select_ops];

  // The timer queues.
  timer_queue_set timer_queues_;

#if defined(AUTOBOOST_ASIO_HAS_IOCP)
  // Does the reactor loop thread need to stop.
  bool stop_thread_;

  // The thread that is running the reactor loop.
  autoboost::asio::detail::thread* thread_;
#endif // defined(AUTOBOOST_ASIO_HAS_IOCP)

  // Whether the service has been shut down.
  bool shutdown_;
};

} // namespace detail
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#include <autoboost/asio/detail/impl/select_reactor.hpp>
#if defined(AUTOBOOST_ASIO_HEADER_ONLY)
# include <autoboost/asio/detail/impl/select_reactor.ipp>
#endif // defined(AUTOBOOST_ASIO_HEADER_ONLY)

#endif // defined(AUTOBOOST_ASIO_HAS_IOCP)
       //   || (!defined(AUTOBOOST_ASIO_HAS_DEV_POLL)
       //       && !defined(AUTOBOOST_ASIO_HAS_EPOLL)
       //       && !defined(AUTOBOOST_ASIO_HAS_KQUEUE)
       //       && !defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME))

#endif // AUTOBOOST_ASIO_DETAIL_SELECT_REACTOR_HPP
