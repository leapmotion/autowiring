//
// ip/address_v4.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_IP_ADDRESS_V4_HPP
#define AUTOBOOST_ASIO_IP_ADDRESS_V4_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>
#include <string>
#include <autoboost/asio/detail/array.hpp>
#include <autoboost/asio/detail/socket_types.hpp>
#include <autoboost/asio/detail/winsock_init.hpp>
#include <autoboost/system/error_code.hpp>

#if !defined(AUTOBOOST_ASIO_NO_IOSTREAM)
# include <iosfwd>
#endif // !defined(AUTOBOOST_ASIO_NO_IOSTREAM)

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace ip {

/// Implements IP version 4 style addresses.
/**
 * The autoboost::asio::ip::address_v4 class provides the ability to use and
 * manipulate IP version 4 addresses.
 *
 * @par Thread Safety
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Unsafe.
 */
class address_v4
{
public:
  /// The type used to represent an address as an array of bytes.
  /**
   * @note This type is defined in terms of the C++0x template @c std::array
   * when it is available. Otherwise, it uses @c autoboost:array.
   */
#if defined(GENERATING_DOCUMENTATION)
  typedef array<unsigned char, 4> bytes_type;
#else
  typedef autoboost::asio::detail::array<unsigned char, 4> bytes_type;
#endif

  /// Default constructor.
  address_v4()
  {
    addr_.s_addr = 0;
  }

  /// Construct an address from raw bytes.
  AUTOBOOST_ASIO_DECL explicit address_v4(const bytes_type& bytes);

  /// Construct an address from a unsigned long in host byte order.
  AUTOBOOST_ASIO_DECL explicit address_v4(unsigned long addr);

  /// Copy constructor.
  address_v4(const address_v4& other)
    : addr_(other.addr_)
  {
  }

#if defined(AUTOBOOST_ASIO_HAS_MOVE)
  /// Move constructor.
  address_v4(address_v4&& other)
    : addr_(other.addr_)
  {
  }
#endif // defined(AUTOBOOST_ASIO_HAS_MOVE)

  /// Assign from another address.
  address_v4& operator=(const address_v4& other)
  {
    addr_ = other.addr_;
    return *this;
  }

#if defined(AUTOBOOST_ASIO_HAS_MOVE)
  /// Move-assign from another address.
  address_v4& operator=(address_v4&& other)
  {
    addr_ = other.addr_;
    return *this;
  }
#endif // defined(AUTOBOOST_ASIO_HAS_MOVE)

  /// Get the address in bytes, in network byte order.
  AUTOBOOST_ASIO_DECL bytes_type to_bytes() const;

  /// Get the address as an unsigned long in host byte order
  AUTOBOOST_ASIO_DECL unsigned long to_ulong() const;

  /// Get the address as a string in dotted decimal format.
  AUTOBOOST_ASIO_DECL std::string to_string() const;

  /// Get the address as a string in dotted decimal format.
  AUTOBOOST_ASIO_DECL std::string to_string(autoboost::system::error_code& ec) const;

  /// Create an address from an IP address string in dotted decimal form.
  AUTOBOOST_ASIO_DECL static address_v4 from_string(const char* str);

  /// Create an address from an IP address string in dotted decimal form.
  AUTOBOOST_ASIO_DECL static address_v4 from_string(
      const char* str, autoboost::system::error_code& ec);

  /// Create an address from an IP address string in dotted decimal form.
  AUTOBOOST_ASIO_DECL static address_v4 from_string(const std::string& str);

  /// Create an address from an IP address string in dotted decimal form.
  AUTOBOOST_ASIO_DECL static address_v4 from_string(
      const std::string& str, autoboost::system::error_code& ec);

  /// Determine whether the address is a loopback address.
  AUTOBOOST_ASIO_DECL bool is_loopback() const;

  /// Determine whether the address is unspecified.
  AUTOBOOST_ASIO_DECL bool is_unspecified() const;

  /// Determine whether the address is a class A address.
  AUTOBOOST_ASIO_DECL bool is_class_a() const;

  /// Determine whether the address is a class B address.
  AUTOBOOST_ASIO_DECL bool is_class_b() const;

  /// Determine whether the address is a class C address.
  AUTOBOOST_ASIO_DECL bool is_class_c() const;

  /// Determine whether the address is a multicast address.
  AUTOBOOST_ASIO_DECL bool is_multicast() const;

  /// Compare two addresses for equality.
  friend bool operator==(const address_v4& a1, const address_v4& a2)
  {
    return a1.addr_.s_addr == a2.addr_.s_addr;
  }

  /// Compare two addresses for inequality.
  friend bool operator!=(const address_v4& a1, const address_v4& a2)
  {
    return a1.addr_.s_addr != a2.addr_.s_addr;
  }

  /// Compare addresses for ordering.
  friend bool operator<(const address_v4& a1, const address_v4& a2)
  {
    return a1.to_ulong() < a2.to_ulong();
  }

  /// Compare addresses for ordering.
  friend bool operator>(const address_v4& a1, const address_v4& a2)
  {
    return a1.to_ulong() > a2.to_ulong();
  }

  /// Compare addresses for ordering.
  friend bool operator<=(const address_v4& a1, const address_v4& a2)
  {
    return a1.to_ulong() <= a2.to_ulong();
  }

  /// Compare addresses for ordering.
  friend bool operator>=(const address_v4& a1, const address_v4& a2)
  {
    return a1.to_ulong() >= a2.to_ulong();
  }

  /// Obtain an address object that represents any address.
  static address_v4 any()
  {
    return address_v4();
  }

  /// Obtain an address object that represents the loopback address.
  static address_v4 loopback()
  {
    return address_v4(0x7F000001);
  }

  /// Obtain an address object that represents the broadcast address.
  static address_v4 broadcast()
  {
    return address_v4(0xFFFFFFFF);
  }

  /// Obtain an address object that represents the broadcast address that
  /// corresponds to the specified address and netmask.
  AUTOBOOST_ASIO_DECL static address_v4 broadcast(
      const address_v4& addr, const address_v4& mask);

  /// Obtain the netmask that corresponds to the address, based on its address
  /// class.
  AUTOBOOST_ASIO_DECL static address_v4 netmask(const address_v4& addr);

private:
  // The underlying IPv4 address.
  autoboost::asio::detail::in4_addr_type addr_;
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
 * @relates autoboost::asio::ip::address_v4
 */
template <typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>& operator<<(
    std::basic_ostream<Elem, Traits>& os, const address_v4& addr);

#endif // !defined(AUTOBOOST_ASIO_NO_IOSTREAM)

} // namespace ip
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#include <autoboost/asio/ip/impl/address_v4.hpp>
#if defined(AUTOBOOST_ASIO_HEADER_ONLY)
# include <autoboost/asio/ip/impl/address_v4.ipp>
#endif // defined(AUTOBOOST_ASIO_HEADER_ONLY)

#endif // AUTOBOOST_ASIO_IP_ADDRESS_V4_HPP
