//
// ip/multicast.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_IP_MULTICAST_HPP
#define AUTOBOOST_ASIO_IP_MULTICAST_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>
#include <cstddef>
#include <autoboost/asio/ip/detail/socket_option.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace ip {
namespace multicast {

/// Socket option to join a multicast group on a specified interface.
/**
 * Implements the IPPROTO_IP/IP_ADD_MEMBERSHIP socket option.
 *
 * @par Examples
 * Setting the option to join a multicast group:
 * @code
 * autoboost::asio::ip::udp::socket socket(io_service);
 * ...
 * autoboost::asio::ip::address multicast_address =
 *   autoboost::asio::ip::address::from_string("225.0.0.1");
 * autoboost::asio::ip::multicast::join_group option(multicast_address);
 * socket.set_option(option);
 * @endcode
 *
 * @par Concepts:
 * SettableSocketOption.
 */
#if defined(GENERATING_DOCUMENTATION)
typedef implementation_defined join_group;
#else
typedef autoboost::asio::ip::detail::socket_option::multicast_request<
  AUTOBOOST_ASIO_OS_DEF(IPPROTO_IP),
  AUTOBOOST_ASIO_OS_DEF(IP_ADD_MEMBERSHIP),
  AUTOBOOST_ASIO_OS_DEF(IPPROTO_IPV6),
  AUTOBOOST_ASIO_OS_DEF(IPV6_JOIN_GROUP)> join_group;
#endif

/// Socket option to leave a multicast group on a specified interface.
/**
 * Implements the IPPROTO_IP/IP_DROP_MEMBERSHIP socket option.
 *
 * @par Examples
 * Setting the option to leave a multicast group:
 * @code
 * autoboost::asio::ip::udp::socket socket(io_service);
 * ...
 * autoboost::asio::ip::address multicast_address =
 *   autoboost::asio::ip::address::from_string("225.0.0.1");
 * autoboost::asio::ip::multicast::leave_group option(multicast_address);
 * socket.set_option(option);
 * @endcode
 *
 * @par Concepts:
 * SettableSocketOption.
 */
#if defined(GENERATING_DOCUMENTATION)
typedef implementation_defined leave_group;
#else
typedef autoboost::asio::ip::detail::socket_option::multicast_request<
  AUTOBOOST_ASIO_OS_DEF(IPPROTO_IP),
  AUTOBOOST_ASIO_OS_DEF(IP_DROP_MEMBERSHIP),
  AUTOBOOST_ASIO_OS_DEF(IPPROTO_IPV6),
  AUTOBOOST_ASIO_OS_DEF(IPV6_LEAVE_GROUP)> leave_group;
#endif

/// Socket option for local interface to use for outgoing multicast packets.
/**
 * Implements the IPPROTO_IP/IP_MULTICAST_IF socket option.
 *
 * @par Examples
 * Setting the option:
 * @code
 * autoboost::asio::ip::udp::socket socket(io_service);
 * ...
 * autoboost::asio::ip::address_v4 local_interface =
 *   autoboost::asio::ip::address_v4::from_string("1.2.3.4");
 * autoboost::asio::ip::multicast::outbound_interface option(local_interface);
 * socket.set_option(option);
 * @endcode
 *
 * @par Concepts:
 * SettableSocketOption.
 */
#if defined(GENERATING_DOCUMENTATION)
typedef implementation_defined outbound_interface;
#else
typedef autoboost::asio::ip::detail::socket_option::network_interface<
  AUTOBOOST_ASIO_OS_DEF(IPPROTO_IP),
  AUTOBOOST_ASIO_OS_DEF(IP_MULTICAST_IF),
  AUTOBOOST_ASIO_OS_DEF(IPPROTO_IPV6),
  AUTOBOOST_ASIO_OS_DEF(IPV6_MULTICAST_IF)> outbound_interface;
#endif

/// Socket option for time-to-live associated with outgoing multicast packets.
/**
 * Implements the IPPROTO_IP/IP_MULTICAST_TTL socket option.
 *
 * @par Examples
 * Setting the option:
 * @code
 * autoboost::asio::ip::udp::socket socket(io_service);
 * ...
 * autoboost::asio::ip::multicast::hops option(4);
 * socket.set_option(option);
 * @endcode
 *
 * @par
 * Getting the current option value:
 * @code
 * autoboost::asio::ip::udp::socket socket(io_service);
 * ...
 * autoboost::asio::ip::multicast::hops option;
 * socket.get_option(option);
 * int ttl = option.value();
 * @endcode
 *
 * @par Concepts:
 * GettableSocketOption, SettableSocketOption.
 */
#if defined(GENERATING_DOCUMENTATION)
typedef implementation_defined hops;
#else
typedef autoboost::asio::ip::detail::socket_option::multicast_hops<
  AUTOBOOST_ASIO_OS_DEF(IPPROTO_IP),
  AUTOBOOST_ASIO_OS_DEF(IP_MULTICAST_TTL),
  AUTOBOOST_ASIO_OS_DEF(IPPROTO_IPV6),
  AUTOBOOST_ASIO_OS_DEF(IPV6_MULTICAST_HOPS)> hops;
#endif

/// Socket option determining whether outgoing multicast packets will be
/// received on the same socket if it is a member of the multicast group.
/**
 * Implements the IPPROTO_IP/IP_MULTICAST_LOOP socket option.
 *
 * @par Examples
 * Setting the option:
 * @code
 * autoboost::asio::ip::udp::socket socket(io_service);
 * ...
 * autoboost::asio::ip::multicast::enable_loopback option(true);
 * socket.set_option(option);
 * @endcode
 *
 * @par
 * Getting the current option value:
 * @code
 * autoboost::asio::ip::udp::socket socket(io_service);
 * ...
 * autoboost::asio::ip::multicast::enable_loopback option;
 * socket.get_option(option);
 * bool is_set = option.value();
 * @endcode
 *
 * @par Concepts:
 * GettableSocketOption, SettableSocketOption.
 */
#if defined(GENERATING_DOCUMENTATION)
typedef implementation_defined enable_loopback;
#else
typedef autoboost::asio::ip::detail::socket_option::multicast_enable_loopback<
  AUTOBOOST_ASIO_OS_DEF(IPPROTO_IP),
  AUTOBOOST_ASIO_OS_DEF(IP_MULTICAST_LOOP),
  AUTOBOOST_ASIO_OS_DEF(IPPROTO_IPV6),
  AUTOBOOST_ASIO_OS_DEF(IPV6_MULTICAST_LOOP)> enable_loopback;
#endif

} // namespace multicast
} // namespace ip
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#endif // AUTOBOOST_ASIO_IP_MULTICAST_HPP
