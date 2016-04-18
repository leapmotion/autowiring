//
// ip/address.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_IP_ADDRESS_HPP
#define AUTOBOOST_ASIO_IP_ADDRESS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>
#include <string>
#include <autoboost/system/error_code.hpp>
#include <autoboost/asio/ip/address_v4.hpp>
#include <autoboost/asio/ip/address_v6.hpp>

#if !defined(AUTOBOOST_ASIO_NO_IOSTREAM)
# include <iosfwd>
#endif // !defined(AUTOBOOST_ASIO_NO_IOSTREAM)

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace ip {

/// Implements version-independent IP addresses.
/**
 * The autoboost::asio::ip::address class provides the ability to use either IP
 * version 4 or version 6 addresses.
 *
 * @par Thread Safety
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Unsafe.
 */
class address
{
public:
  /// Default constructor.
  AUTOBOOST_ASIO_DECL address();

  /// Construct an address from an IPv4 address.
  AUTOBOOST_ASIO_DECL address(const autoboost::asio::ip::address_v4& ipv4_address);

  /// Construct an address from an IPv6 address.
  AUTOBOOST_ASIO_DECL address(const autoboost::asio::ip::address_v6& ipv6_address);

  /// Copy constructor.
  AUTOBOOST_ASIO_DECL address(const address& other);

#if defined(AUTOBOOST_ASIO_HAS_MOVE)
  /// Move constructor.
  AUTOBOOST_ASIO_DECL address(address&& other);
#endif // defined(AUTOBOOST_ASIO_HAS_MOVE)

  /// Assign from another address.
  AUTOBOOST_ASIO_DECL address& operator=(const address& other);

#if defined(AUTOBOOST_ASIO_HAS_MOVE)
  /// Move-assign from another address.
  AUTOBOOST_ASIO_DECL address& operator=(address&& other);
#endif // defined(AUTOBOOST_ASIO_HAS_MOVE)

  /// Assign from an IPv4 address.
  AUTOBOOST_ASIO_DECL address& operator=(
      const autoboost::asio::ip::address_v4& ipv4_address);

  /// Assign from an IPv6 address.
  AUTOBOOST_ASIO_DECL address& operator=(
      const autoboost::asio::ip::address_v6& ipv6_address);

  /// Get whether the address is an IP version 4 address.
  bool is_v4() const
  {
    return type_ == ipv4;
  }

  /// Get whether the address is an IP version 6 address.
  bool is_v6() const
  {
    return type_ == ipv6;
  }

  /// Get the address as an IP version 4 address.
  AUTOBOOST_ASIO_DECL autoboost::asio::ip::address_v4 to_v4() const;

  /// Get the address as an IP version 6 address.
  AUTOBOOST_ASIO_DECL autoboost::asio::ip::address_v6 to_v6() const;

  /// Get the address as a string in dotted decimal format.
  AUTOBOOST_ASIO_DECL std::string to_string() const;

  /// Get the address as a string in dotted decimal format.
  AUTOBOOST_ASIO_DECL std::string to_string(autoboost::system::error_code& ec) const;

  /// Create an address from an IPv4 address string in dotted decimal form,
  /// or from an IPv6 address in hexadecimal notation.
  AUTOBOOST_ASIO_DECL static address from_string(const char* str);

  /// Create an address from an IPv4 address string in dotted decimal form,
  /// or from an IPv6 address in hexadecimal notation.
  AUTOBOOST_ASIO_DECL static address from_string(
      const char* str, autoboost::system::error_code& ec);

  /// Create an address from an IPv4 address string in dotted decimal form,
  /// or from an IPv6 address in hexadecimal notation.
  AUTOBOOST_ASIO_DECL static address from_string(const std::string& str);

  /// Create an address from an IPv4 address string in dotted decimal form,
  /// or from an IPv6 address in hexadecimal notation.
  AUTOBOOST_ASIO_DECL static address from_string(
      const std::string& str, autoboost::system::error_code& ec);

  /// Determine whether the address is a loopback address.
  AUTOBOOST_ASIO_DECL bool is_loopback() const;

  /// Determine whether the address is unspecified.
  AUTOBOOST_ASIO_DECL bool is_unspecified() const;

  /// Determine whether the address is a multicast address.
  AUTOBOOST_ASIO_DECL bool is_multicast() const;

  /// Compare two addresses for equality.
  AUTOBOOST_ASIO_DECL friend bool operator==(const address& a1, const address& a2);

  /// Compare two addresses for inequality.
  friend bool operator!=(const address& a1, const address& a2)
  {
    return !(a1 == a2);
  }

  /// Compare addresses for ordering.
  AUTOBOOST_ASIO_DECL friend bool operator<(const address& a1, const address& a2);

  /// Compare addresses for ordering.
  friend bool operator>(const address& a1, const address& a2)
  {
    return a2 < a1;
  }

  /// Compare addresses for ordering.
  friend bool operator<=(const address& a1, const address& a2)
  {
    return !(a2 < a1);
  }

  /// Compare addresses for ordering.
  friend bool operator>=(const address& a1, const address& a2)
  {
    return !(a1 < a2);
  }

private:
  // The type of the address.
  enum { ipv4, ipv6 } type_;

  // The underlying IPv4 address.
  autoboost::asio::ip::address_v4 ipv4_address_;

  // The underlying IPv6 address.
  autoboost::asio::ip::address_v6 ipv6_address_;
};

#if !defined(AUTOBOOST_ASIO_NO_IOSTREAM)

/// Output an address as a string.
/**
 * Used to output a human-readable string for a specified address.
 *
 * @param os The output stream to which the string will be written.
 *
 * @param addr The address to be written.
 *
 * @return The output stream.
 *
 * @relates autoboost::asio::ip::address
 */
template <typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>& operator<<(
    std::basic_ostream<Elem, Traits>& os, const address& addr);

#endif // !defined(AUTOBOOST_ASIO_NO_IOSTREAM)

} // namespace ip
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#include <autoboost/asio/ip/impl/address.hpp>
#if defined(AUTOBOOST_ASIO_HEADER_ONLY)
# include <autoboost/asio/ip/impl/address.ipp>
#endif // defined(AUTOBOOST_ASIO_HEADER_ONLY)

#endif // AUTOBOOST_ASIO_IP_ADDRESS_HPP
