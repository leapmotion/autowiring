//
// ip/detail/impl/endpoint.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_IP_DETAIL_IMPL_ENDPOINT_IPP
#define AUTOBOOST_ASIO_IP_DETAIL_IMPL_ENDPOINT_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>
#include <cstring>
#if !defined(AUTOBOOST_ASIO_NO_IOSTREAM)
# include <sstream>
#endif // !defined(AUTOBOOST_ASIO_NO_IOSTREAM)
#include <autoboost/asio/detail/socket_ops.hpp>
#include <autoboost/asio/detail/throw_error.hpp>
#include <autoboost/asio/error.hpp>
#include <autoboost/asio/ip/detail/endpoint.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace ip {
namespace detail {

endpoint::endpoint()
  : data_()
{
  data_.v4.sin_family = AUTOBOOST_ASIO_OS_DEF(AF_INET);
  data_.v4.sin_port = 0;
  data_.v4.sin_addr.s_addr = AUTOBOOST_ASIO_OS_DEF(INADDR_ANY);
}

endpoint::endpoint(int family, unsigned short port_num)
  : data_()
{
  using namespace std; // For memcpy.
  if (family == AUTOBOOST_ASIO_OS_DEF(AF_INET))
  {
    data_.v4.sin_family = AUTOBOOST_ASIO_OS_DEF(AF_INET);
    data_.v4.sin_port =
      autoboost::asio::detail::socket_ops::host_to_network_short(port_num);
    data_.v4.sin_addr.s_addr = AUTOBOOST_ASIO_OS_DEF(INADDR_ANY);
  }
  else
  {
    data_.v6.sin6_family = AUTOBOOST_ASIO_OS_DEF(AF_INET6);
    data_.v6.sin6_port =
      autoboost::asio::detail::socket_ops::host_to_network_short(port_num);
    data_.v6.sin6_flowinfo = 0;
    data_.v6.sin6_addr.s6_addr[0] = 0; data_.v6.sin6_addr.s6_addr[1] = 0;
    data_.v6.sin6_addr.s6_addr[2] = 0; data_.v6.sin6_addr.s6_addr[3] = 0;
    data_.v6.sin6_addr.s6_addr[4] = 0; data_.v6.sin6_addr.s6_addr[5] = 0;
    data_.v6.sin6_addr.s6_addr[6] = 0; data_.v6.sin6_addr.s6_addr[7] = 0;
    data_.v6.sin6_addr.s6_addr[8] = 0; data_.v6.sin6_addr.s6_addr[9] = 0;
    data_.v6.sin6_addr.s6_addr[10] = 0; data_.v6.sin6_addr.s6_addr[11] = 0;
    data_.v6.sin6_addr.s6_addr[12] = 0; data_.v6.sin6_addr.s6_addr[13] = 0;
    data_.v6.sin6_addr.s6_addr[14] = 0; data_.v6.sin6_addr.s6_addr[15] = 0;
    data_.v6.sin6_scope_id = 0;
  }
}

endpoint::endpoint(const autoboost::asio::ip::address& addr,
    unsigned short port_num)
  : data_()
{
  using namespace std; // For memcpy.
  if (addr.is_v4())
  {
    data_.v4.sin_family = AUTOBOOST_ASIO_OS_DEF(AF_INET);
    data_.v4.sin_port =
      autoboost::asio::detail::socket_ops::host_to_network_short(port_num);
    data_.v4.sin_addr.s_addr =
      autoboost::asio::detail::socket_ops::host_to_network_long(
          static_cast<autoboost::asio::detail::u_long_type>(
            addr.to_v4().to_ulong()));
  }
  else
  {
    data_.v6.sin6_family = AUTOBOOST_ASIO_OS_DEF(AF_INET6);
    data_.v6.sin6_port =
      autoboost::asio::detail::socket_ops::host_to_network_short(port_num);
    data_.v6.sin6_flowinfo = 0;
    autoboost::asio::ip::address_v6 v6_addr = addr.to_v6();
    autoboost::asio::ip::address_v6::bytes_type bytes = v6_addr.to_bytes();
    memcpy(data_.v6.sin6_addr.s6_addr, bytes.data(), 16);
    data_.v6.sin6_scope_id =
      static_cast<autoboost::asio::detail::u_long_type>(
        v6_addr.scope_id());
  }
}

void endpoint::resize(std::size_t new_size)
{
  if (new_size > sizeof(autoboost::asio::detail::sockaddr_storage_type))
  {
    autoboost::system::error_code ec(autoboost::asio::error::invalid_argument);
    autoboost::asio::detail::throw_error(ec);
  }
}

unsigned short endpoint::port() const
{
  if (is_v4())
  {
    return autoboost::asio::detail::socket_ops::network_to_host_short(
        data_.v4.sin_port);
  }
  else
  {
    return autoboost::asio::detail::socket_ops::network_to_host_short(
        data_.v6.sin6_port);
  }
}

void endpoint::port(unsigned short port_num)
{
  if (is_v4())
  {
    data_.v4.sin_port
      = autoboost::asio::detail::socket_ops::host_to_network_short(port_num);
  }
  else
  {
    data_.v6.sin6_port
      = autoboost::asio::detail::socket_ops::host_to_network_short(port_num);
  }
}

autoboost::asio::ip::address endpoint::address() const
{
  using namespace std; // For memcpy.
  if (is_v4())
  {
    return autoboost::asio::ip::address_v4(
        autoboost::asio::detail::socket_ops::network_to_host_long(
          data_.v4.sin_addr.s_addr));
  }
  else
  {
    autoboost::asio::ip::address_v6::bytes_type bytes;
#if defined(AUTOBOOST_ASIO_HAS_STD_ARRAY)
    memcpy(bytes.data(), data_.v6.sin6_addr.s6_addr, 16);
#else // defined(AUTOBOOST_ASIO_HAS_STD_ARRAY)
    memcpy(bytes.elems, data_.v6.sin6_addr.s6_addr, 16);
#endif // defined(AUTOBOOST_ASIO_HAS_STD_ARRAY)
    return autoboost::asio::ip::address_v6(bytes, data_.v6.sin6_scope_id);
  }
}

void endpoint::address(const autoboost::asio::ip::address& addr)
{
  endpoint tmp_endpoint(addr, port());
  data_ = tmp_endpoint.data_;
}

bool operator==(const endpoint& e1, const endpoint& e2)
{
  return e1.address() == e2.address() && e1.port() == e2.port();
}

bool operator<(const endpoint& e1, const endpoint& e2)
{
  if (e1.address() < e2.address())
    return true;
  if (e1.address() != e2.address())
    return false;
  return e1.port() < e2.port();
}

#if !defined(AUTOBOOST_ASIO_NO_IOSTREAM)
std::string endpoint::to_string(autoboost::system::error_code& ec) const
{
  std::string a = address().to_string(ec);
  if (ec)
    return std::string();

  std::ostringstream tmp_os;
  tmp_os.imbue(std::locale::classic());
  if (is_v4())
    tmp_os << a;
  else
    tmp_os << '[' << a << ']';
  tmp_os << ':' << port();

  return tmp_os.str();
}
#endif // !defined(AUTOBOOST_ASIO_NO_IOSTREAM)

} // namespace detail
} // namespace ip
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#endif // AUTOBOOST_ASIO_IP_DETAIL_IMPL_ENDPOINT_IPP
