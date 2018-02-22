//
// detail/strand_service.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_STRAND_SERVICE_HPP
#define AUTOBOOST_ASIO_DETAIL_STRAND_SERVICE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>
#include <autoboost/asio/io_service.hpp>
#include <autoboost/asio/detail/mutex.hpp>
#include <autoboost/asio/detail/op_queue.hpp>
#include <autoboost/asio/detail/operation.hpp>
#include <autoboost/asio/detail/scoped_ptr.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace detail {

// Default service implementation for a strand.
class strand_service
  : public autoboost::asio::detail::service_base<strand_service>
{
private:
  // Helper class to re-post the strand on exit.
  struct on_do_complete_exit;

  // Helper class to re-post the strand on exit.
  struct on_dispatch_exit;

public:

  // The underlying implementation of a strand.
  class strand_impl
    : public operation
  {
  public:
    strand_impl();

  private:
    // Only this service will have access to the internal values.
    friend class strand_service;
    friend struct on_do_complete_exit;
    friend struct on_dispatch_exit;

    // Mutex to protect access to internal data.
    autoboost::asio::detail::mutex mutex_;

    // Indicates whether the strand is currently "locked" by a handler. This
    // means that there is a handler upcall in progress, or that the strand
    // itself has been scheduled in order to invoke some pending handlers.
    bool locked_;

    // The handlers that are waiting on the strand but should not be run until
    // after the next time the strand is scheduled. This queue must only be
    // modified while the mutex is locked.
    op_queue<operation> waiting_queue_;

    // The handlers that are ready to be run. Logically speaking, these are the
    // handlers that hold the strand's lock. The ready queue is only modified
    // from within the strand and so may be accessed without locking the mutex.
    op_queue<operation> ready_queue_;
  };

  typedef strand_impl* implementation_type;

  // Construct a new strand service for the specified io_service.
  AUTOBOOST_ASIO_DECL explicit strand_service(autoboost::asio::io_service& io_service);

  // Destroy all user-defined handler objects owned by the service.
  AUTOBOOST_ASIO_DECL void shutdown_service();

  // Construct a new strand implementation.
  AUTOBOOST_ASIO_DECL void construct(implementation_type& impl);

  // Request the io_service to invoke the given handler.
  template <typename Handler>
  void dispatch(implementation_type& impl, Handler& handler);

  // Request the io_service to invoke the given handler and return immediately.
  template <typename Handler>
  void post(implementation_type& impl, Handler& handler);

  // Determine whether the strand is running in the current thread.
  AUTOBOOST_ASIO_DECL bool running_in_this_thread(
      const implementation_type& impl) const;

private:
  // Helper function to dispatch a handler. Returns true if the handler should
  // be dispatched immediately.
  AUTOBOOST_ASIO_DECL bool do_dispatch(implementation_type& impl, operation* op);

  // Helper fiunction to post a handler.
  AUTOBOOST_ASIO_DECL void do_post(implementation_type& impl,
      operation* op, bool is_continuation);

  AUTOBOOST_ASIO_DECL static void do_complete(io_service_impl* owner,
      operation* base, const autoboost::system::error_code& ec,
      std::size_t bytes_transferred);

  // The io_service implementation used to post completions.
  io_service_impl& io_service_;

  // Mutex to protect access to the array of implementations.
  autoboost::asio::detail::mutex mutex_;

  // Number of implementations shared between all strand objects.
#if defined(AUTOBOOST_ASIO_STRAND_IMPLEMENTATIONS)
  enum { num_implementations = AUTOBOOST_ASIO_STRAND_IMPLEMENTATIONS };
#else // defined(AUTOBOOST_ASIO_STRAND_IMPLEMENTATIONS)
  enum { num_implementations = 193 };
#endif // defined(AUTOBOOST_ASIO_STRAND_IMPLEMENTATIONS)

  // Pool of implementations.
  scoped_ptr<strand_impl> implementations_[num_implementations];

  // Extra value used when hashing to prevent recycled memory locations from
  // getting the same strand implementation.
  std::size_t salt_;
};

} // namespace detail
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#include <autoboost/asio/detail/impl/strand_service.hpp>
#if defined(AUTOBOOST_ASIO_HEADER_ONLY)
# include <autoboost/asio/detail/impl/strand_service.ipp>
#endif // defined(AUTOBOOST_ASIO_HEADER_ONLY)

#endif // AUTOBOOST_ASIO_DETAIL_STRAND_SERVICE_HPP
