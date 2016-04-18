//
// ip/resolver_service.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_IP_RESOLVER_SERVICE_HPP
#define AUTOBOOST_ASIO_IP_RESOLVER_SERVICE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>
#include <autoboost/asio/async_result.hpp>
#include <autoboost/system/error_code.hpp>
#include <autoboost/asio/io_service.hpp>
#include <autoboost/asio/ip/basic_resolver_iterator.hpp>
#include <autoboost/asio/ip/basic_resolver_query.hpp>

#if defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)
# include <autoboost/asio/detail/winrt_resolver_service.hpp>
#else
# include <autoboost/asio/detail/resolver_service.hpp>
#endif

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace ip {

/// Default service implementation for a resolver.
template <typename InternetProtocol>
class resolver_service
#if defined(GENERATING_DOCUMENTATION)
  : public autoboost::asio::io_service::service
#else
  : public autoboost::asio::detail::service_base<
      resolver_service<InternetProtocol> >
#endif
{
public:
#if defined(GENERATING_DOCUMENTATION)
  /// The unique service identifier.
  static autoboost::asio::io_service::id id;
#endif

  /// The protocol type.
  typedef InternetProtocol protocol_type;

  /// The endpoint type.
  typedef typename InternetProtocol::endpoint endpoint_type;

  /// The query type.
  typedef basic_resolver_query<InternetProtocol> query_type;

  /// The iterator type.
  typedef basic_resolver_iterator<InternetProtocol> iterator_type;

private:
  // The type of the platform-specific implementation.
#if defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)
  typedef autoboost::asio::detail::winrt_resolver_service<InternetProtocol>
    service_impl_type;
#else
  typedef autoboost::asio::detail::resolver_service<InternetProtocol>
    service_impl_type;
#endif

public:
  /// The type of a resolver implementation.
#if defined(GENERATING_DOCUMENTATION)
  typedef implementation_defined implementation_type;
#else
  typedef typename service_impl_type::implementation_type implementation_type;
#endif

  /// Construct a new resolver service for the specified io_service.
  explicit resolver_service(autoboost::asio::io_service& io_service)
    : autoboost::asio::detail::service_base<
        resolver_service<InternetProtocol> >(io_service),
      service_impl_(io_service)
  {
  }

  /// Construct a new resolver implementation.
  void construct(implementation_type& impl)
  {
    service_impl_.construct(impl);
  }

  /// Destroy a resolver implementation.
  void destroy(implementation_type& impl)
  {
    service_impl_.destroy(impl);
  }

  /// Cancel pending asynchronous operations.
  void cancel(implementation_type& impl)
  {
    service_impl_.cancel(impl);
  }

  /// Resolve a query to a list of entries.
  iterator_type resolve(implementation_type& impl, const query_type& query,
      autoboost::system::error_code& ec)
  {
    return service_impl_.resolve(impl, query, ec);
  }

  /// Asynchronously resolve a query to a list of entries.
  template <typename ResolveHandler>
  AUTOBOOST_ASIO_INITFN_RESULT_TYPE(ResolveHandler,
      void (autoboost::system::error_code, iterator_type))
  async_resolve(implementation_type& impl, const query_type& query,
      AUTOBOOST_ASIO_MOVE_ARG(ResolveHandler) handler)
  {
    autoboost::asio::detail::async_result_init<
      ResolveHandler, void (autoboost::system::error_code, iterator_type)> init(
        AUTOBOOST_ASIO_MOVE_CAST(ResolveHandler)(handler));

    service_impl_.async_resolve(impl, query, init.handler);

    return init.result.get();
  }

  /// Resolve an endpoint to a list of entries.
  iterator_type resolve(implementation_type& impl,
      const endpoint_type& endpoint, autoboost::system::error_code& ec)
  {
    return service_impl_.resolve(impl, endpoint, ec);
  }

  /// Asynchronously resolve an endpoint to a list of entries.
  template <typename ResolveHandler>
  AUTOBOOST_ASIO_INITFN_RESULT_TYPE(ResolveHandler,
      void (autoboost::system::error_code, iterator_type))
  async_resolve(implementation_type& impl, const endpoint_type& endpoint,
      AUTOBOOST_ASIO_MOVE_ARG(ResolveHandler) handler)
  {
    autoboost::asio::detail::async_result_init<
      ResolveHandler, void (autoboost::system::error_code, iterator_type)> init(
        AUTOBOOST_ASIO_MOVE_CAST(ResolveHandler)(handler));

    service_impl_.async_resolve(impl, endpoint, init.handler);

    return init.result.get();
  }

private:
  // Destroy all user-defined handler objects owned by the service.
  void shutdown_service()
  {
    service_impl_.shutdown_service();
  }

  // Perform any fork-related housekeeping.
  void fork_service(autoboost::asio::io_service::fork_event event)
  {
    service_impl_.fork_service(event);
  }

  // The platform-specific implementation.
  service_impl_type service_impl_;
};

} // namespace ip
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#endif // AUTOBOOST_ASIO_IP_RESOLVER_SERVICE_HPP
