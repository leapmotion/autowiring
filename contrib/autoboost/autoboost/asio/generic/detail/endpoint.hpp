//
// generic/detail/endpoint.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_GENERIC_DETAIL_ENDPOINT_HPP
#define AUTOBOOST_ASIO_GENERIC_DETAIL_ENDPOINT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#include <cstddef>
#include <autoboost/asio/detail/socket_types.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace generic {
namespace detail {

// Helper class for implementing a generic socket endpoint.
class endpoint
{
public:
  // Default constructor.
  AUTOBOOST_ASIO_DECL endpoint();

  // Construct an endpoint from the specified raw bytes.
  AUTOBOOST_ASIO_DECL endpoint(const void* sock_addr,
      std::size_t sock_addr_size, int sock_protocol);

  // Copy constructor.
  endpoint(const endpoint& other)
    : data_(other.data_),
      size_(other.size_),
      protocol_(other.protocol_)
  {
  }

  // Assign from another endpoint.
  endpoint& operator=(const endpoint& other)
  {
    data_ = other.data_;
    size_ = other.size_;
    protocol_ = other.protocol_;
    return *this;
  }

  // Get the address family associated with the endpoint.
  int family() const
  {
    return data_.base.sa_family;
  }

  // Get the socket protocol associated with the endpoint.
  int protocol() const
  {
    return protocol_;
  }

  // Get the underlying endpoint in the native type.
  autoboost::asio::detail::socket_addr_type* data()
  {
    return &data_.base;
  }

  // Get the underlying endpoint in the native type.
  const autoboost::asio::detail::socket_addr_type* data() const
  {
    return &data_.base;
  }

  // Get the underlying size of the endpoint in the native type.
  std::size_t size() const
  {
    return size_;
  }

  // Set the underlying size of the endpoint in the native type.
  AUTOBOOST_ASIO_DECL void resize(std::size_t size);

  // Get the capacity of the endpoint in the native type.
  std::size_t capacity() const
  {
    return sizeof(autoboost::asio::detail::sockaddr_storage_type);
  }

  // Compare two endpoints for equality.
  AUTOBOOST_ASIO_DECL friend bool operator==(
      const endpoint& e1, const endpoint& e2);

  // Compare endpoints for ordering.
  AUTOBOOST_ASIO_DECL friend bool operator<(
      const endpoint& e1, const endpoint& e2);

private:
  // The underlying socket address.
  union data_union
  {
    autoboost::asio::detail::socket_addr_type base;
    autoboost::asio::detail::sockaddr_storage_type generic;
  } data_;

  // The length of the socket address stored in the endpoint.
  std::size_t size_;

  // The socket protocol associated with the endpoint.
  int protocol_;

  // Initialise with a specified memory.
  AUTOBOOST_ASIO_DECL void init(const void* sock_addr,
      std::size_t sock_addr_size, int sock_protocol);
};

} // namespace detail
} // namespace generic
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#if defined(AUTOBOOST_ASIO_HEADER_ONLY)
# include <autoboost/asio/generic/detail/impl/endpoint.ipp>
#endif // defined(AUTOBOOST_ASIO_HEADER_ONLY)

#endif // AUTOBOOST_ASIO_GENERIC_DETAIL_ENDPOINT_HPP
