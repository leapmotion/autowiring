//
// local/connect_pair.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_LOCAL_CONNECT_PAIR_HPP
#define AUTOBOOST_ASIO_LOCAL_CONNECT_PAIR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_HAS_LOCAL_SOCKETS) \
  || defined(GENERATING_DOCUMENTATION)

#include <autoboost/asio/basic_socket.hpp>
#include <autoboost/asio/detail/socket_ops.hpp>
#include <autoboost/asio/detail/throw_error.hpp>
#include <autoboost/asio/error.hpp>
#include <autoboost/asio/local/basic_endpoint.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace local {

/// Create a pair of connected sockets.
template <typename Protocol, typename SocketService1, typename SocketService2>
void connect_pair(
    basic_socket<Protocol, SocketService1>& socket1,
    basic_socket<Protocol, SocketService2>& socket2);

/// Create a pair of connected sockets.
template <typename Protocol, typename SocketService1, typename SocketService2>
autoboost::system::error_code connect_pair(
    basic_socket<Protocol, SocketService1>& socket1,
    basic_socket<Protocol, SocketService2>& socket2,
    autoboost::system::error_code& ec);

template <typename Protocol, typename SocketService1, typename SocketService2>
inline void connect_pair(
    basic_socket<Protocol, SocketService1>& socket1,
    basic_socket<Protocol, SocketService2>& socket2)
{
  autoboost::system::error_code ec;
  connect_pair(socket1, socket2, ec);
  autoboost::asio::detail::throw_error(ec, "connect_pair");
}

template <typename Protocol, typename SocketService1, typename SocketService2>
inline autoboost::system::error_code connect_pair(
    basic_socket<Protocol, SocketService1>& socket1,
    basic_socket<Protocol, SocketService2>& socket2,
    autoboost::system::error_code& ec)
{
  // Check that this function is only being used with a UNIX domain socket.
  autoboost::asio::local::basic_endpoint<Protocol>* tmp
    = static_cast<typename Protocol::endpoint*>(0);
  (void)tmp;

  Protocol protocol;
  autoboost::asio::detail::socket_type sv[2];
  if (autoboost::asio::detail::socket_ops::socketpair(protocol.family(),
        protocol.type(), protocol.protocol(), sv, ec)
      == autoboost::asio::detail::socket_error_retval)
    return ec;

  if (socket1.assign(protocol, sv[0], ec))
  {
    autoboost::system::error_code temp_ec;
    autoboost::asio::detail::socket_ops::state_type state[2] = { 0, 0 };
    autoboost::asio::detail::socket_ops::close(sv[0], state[0], true, temp_ec);
    autoboost::asio::detail::socket_ops::close(sv[1], state[1], true, temp_ec);
    return ec;
  }

  if (socket2.assign(protocol, sv[1], ec))
  {
    autoboost::system::error_code temp_ec;
    socket1.close(temp_ec);
    autoboost::asio::detail::socket_ops::state_type state = 0;
    autoboost::asio::detail::socket_ops::close(sv[1], state, true, temp_ec);
    return ec;
  }

  return ec;
}

} // namespace local
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#endif // defined(AUTOBOOST_ASIO_HAS_LOCAL_SOCKETS)
       //   || defined(GENERATING_DOCUMENTATION)

#endif // AUTOBOOST_ASIO_LOCAL_CONNECT_PAIR_HPP
