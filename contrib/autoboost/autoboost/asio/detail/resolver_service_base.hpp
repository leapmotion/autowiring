//
// detail/resolver_service_base.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_RESOLVER_SERVICE_BASE_HPP
#define AUTOBOOST_ASIO_DETAIL_RESOLVER_SERVICE_BASE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>
#include <autoboost/asio/error.hpp>
#include <autoboost/asio/io_service.hpp>
#include <autoboost/asio/detail/mutex.hpp>
#include <autoboost/asio/detail/noncopyable.hpp>
#include <autoboost/asio/detail/operation.hpp>
#include <autoboost/asio/detail/socket_ops.hpp>
#include <autoboost/asio/detail/socket_types.hpp>
#include <autoboost/asio/detail/scoped_ptr.hpp>
#include <autoboost/asio/detail/thread.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace detail {

class resolver_service_base
{
public:
  // The implementation type of the resolver. A cancellation token is used to
  // indicate to the background thread that the operation has been cancelled.
  typedef socket_ops::shared_cancel_token_type implementation_type;

  // Constructor.
  AUTOBOOST_ASIO_DECL resolver_service_base(autoboost::asio::io_service& io_service);

  // Destructor.
  AUTOBOOST_ASIO_DECL ~resolver_service_base();

  // Destroy all user-defined handler objects owned by the service.
  AUTOBOOST_ASIO_DECL void shutdown_service();

  // Perform any fork-related housekeeping.
  AUTOBOOST_ASIO_DECL void fork_service(
      autoboost::asio::io_service::fork_event fork_ev);

  // Construct a new resolver implementation.
  AUTOBOOST_ASIO_DECL void construct(implementation_type& impl);

  // Destroy a resolver implementation.
  AUTOBOOST_ASIO_DECL void destroy(implementation_type&);

  // Cancel pending asynchronous operations.
  AUTOBOOST_ASIO_DECL void cancel(implementation_type& impl);

protected:
  // Helper function to start an asynchronous resolve operation.
  AUTOBOOST_ASIO_DECL void start_resolve_op(operation* op);

#if !defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)
  // Helper class to perform exception-safe cleanup of addrinfo objects.
  class auto_addrinfo
    : private autoboost::asio::detail::noncopyable
  {
  public:
    explicit auto_addrinfo(autoboost::asio::detail::addrinfo_type* ai)
      : ai_(ai)
    {
    }

    ~auto_addrinfo()
    {
      if (ai_)
        socket_ops::freeaddrinfo(ai_);
    }

    operator autoboost::asio::detail::addrinfo_type*()
    {
      return ai_;
    }

  private:
    autoboost::asio::detail::addrinfo_type* ai_;
  };
#endif // !defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)

  // Helper class to run the work io_service in a thread.
  class work_io_service_runner;

  // Start the work thread if it's not already running.
  AUTOBOOST_ASIO_DECL void start_work_thread();

  // The io_service implementation used to post completions.
  io_service_impl& io_service_impl_;

private:
  // Mutex to protect access to internal data.
  autoboost::asio::detail::mutex mutex_;

  // Private io_service used for performing asynchronous host resolution.
  autoboost::asio::detail::scoped_ptr<autoboost::asio::io_service> work_io_service_;

  // The work io_service implementation used to post completions.
  io_service_impl& work_io_service_impl_;

  // Work for the private io_service to perform.
  autoboost::asio::detail::scoped_ptr<autoboost::asio::io_service::work> work_;

  // Thread used for running the work io_service's run loop.
  autoboost::asio::detail::scoped_ptr<autoboost::asio::detail::thread> work_thread_;
};

} // namespace detail
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#if defined(AUTOBOOST_ASIO_HEADER_ONLY)
# include <autoboost/asio/detail/impl/resolver_service_base.ipp>
#endif // defined(AUTOBOOST_ASIO_HEADER_ONLY)

#endif // AUTOBOOST_ASIO_DETAIL_RESOLVER_SERVICE_BASE_HPP
