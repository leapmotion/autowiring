//
// detail/winrt_resolver_service.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_WINRT_RESOLVER_SERVICE_HPP
#define AUTOBOOST_ASIO_DETAIL_WINRT_RESOLVER_SERVICE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)

#include <autoboost/asio/ip/basic_resolver_iterator.hpp>
#include <autoboost/asio/ip/basic_resolver_query.hpp>
#include <autoboost/asio/detail/addressof.hpp>
#include <autoboost/asio/detail/bind_handler.hpp>
#include <autoboost/asio/detail/socket_ops.hpp>
#include <autoboost/asio/detail/winrt_async_manager.hpp>
#include <autoboost/asio/detail/winrt_resolve_op.hpp>
#include <autoboost/asio/detail/winrt_utils.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace detail {

template <typename Protocol>
class winrt_resolver_service
{
public:
  // The implementation type of the resolver. A cancellation token is used to
  // indicate to the asynchronous operation that the operation has been
  // cancelled.
  typedef socket_ops::shared_cancel_token_type implementation_type;

  // The endpoint type.
  typedef typename Protocol::endpoint endpoint_type;

  // The query type.
  typedef autoboost::asio::ip::basic_resolver_query<Protocol> query_type;

  // The iterator type.
  typedef autoboost::asio::ip::basic_resolver_iterator<Protocol> iterator_type;

  // Constructor.
  winrt_resolver_service(autoboost::asio::io_service& io_service)
    : io_service_(use_service<io_service_impl>(io_service)),
      async_manager_(use_service<winrt_async_manager>(io_service))
  {
  }

  // Destructor.
  ~winrt_resolver_service()
  {
  }

  // Destroy all user-defined handler objects owned by the service.
  void shutdown_service()
  {
  }

  // Perform any fork-related housekeeping.
  void fork_service(autoboost::asio::io_service::fork_event)
  {
  }

  // Construct a new resolver implementation.
  void construct(implementation_type&)
  {
  }

  // Destroy a resolver implementation.
  void destroy(implementation_type&)
  {
  }

  // Cancel pending asynchronous operations.
  void cancel(implementation_type&)
  {
  }

  // Resolve a query to a list of entries.
  iterator_type resolve(implementation_type&,
      const query_type& query, autoboost::system::error_code& ec)
  {
    try
    {
      using namespace Windows::Networking::Sockets;
      auto endpoint_pairs = async_manager_.sync(
          DatagramSocket::GetEndpointPairsAsync(
            winrt_utils::host_name(query.host_name()),
            winrt_utils::string(query.service_name())), ec);

      if (ec)
        return iterator_type();

      return iterator_type::create(
          endpoint_pairs, query.hints(),
          query.host_name(), query.service_name());
    }
    catch (Platform::Exception^ e)
    {
      ec = autoboost::system::error_code(e->HResult,
          autoboost::system::system_category());
      return iterator_type();
    }
  }

  // Asynchronously resolve a query to a list of entries.
  template <typename Handler>
  void async_resolve(implementation_type&,
      const query_type& query, Handler& handler)
  {
    bool is_continuation =
      autoboost_asio_handler_cont_helpers::is_continuation(handler);

    // Allocate and construct an operation to wrap the handler.
    typedef winrt_resolve_op<Protocol, Handler> op;
    typename op::ptr p = { autoboost::asio::detail::addressof(handler),
      autoboost_asio_handler_alloc_helpers::allocate(
        sizeof(op), handler), 0 };
    p.p = new (p.v) op(query, handler);

    AUTOBOOST_ASIO_HANDLER_CREATION((p.p, "resolver", &impl, "async_resolve"));

    try
    {
      using namespace Windows::Networking::Sockets;
      async_manager_.async(DatagramSocket::GetEndpointPairsAsync(
            winrt_utils::host_name(query.host_name()),
            winrt_utils::string(query.service_name())), p.p);
      p.v = p.p = 0;
    }
    catch (Platform::Exception^ e)
    {
      p.p->ec_ = autoboost::system::error_code(
          e->HResult, autoboost::system::system_category());
      io_service_.post_immediate_completion(p.p, is_continuation);
      p.v = p.p = 0;
    }
  }

  // Resolve an endpoint to a list of entries.
  iterator_type resolve(implementation_type&,
      const endpoint_type&, autoboost::system::error_code& ec)
  {
    ec = autoboost::asio::error::operation_not_supported;
    return iterator_type();
  }

  // Asynchronously resolve an endpoint to a list of entries.
  template <typename Handler>
  void async_resolve(implementation_type&,
      const endpoint_type&, Handler& handler)
  {
    autoboost::system::error_code ec = autoboost::asio::error::operation_not_supported;
    const iterator_type iterator;
    io_service_.get_io_service().post(
        detail::bind_handler(handler, ec, iterator));
  }

private:
  io_service_impl& io_service_;
  winrt_async_manager& async_manager_;
};

} // namespace detail
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#endif // defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)

#endif // AUTOBOOST_ASIO_DETAIL_WINRT_RESOLVER_SERVICE_HPP
