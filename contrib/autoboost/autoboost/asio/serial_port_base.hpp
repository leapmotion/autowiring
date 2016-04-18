//
// serial_port_base.hpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2008 Rep Invariant Systems, Inc. (info@repinvariant.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_SERIAL_PORT_BASE_HPP
#define AUTOBOOST_ASIO_SERIAL_PORT_BASE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_HAS_SERIAL_PORT) \
  || defined(GENERATING_DOCUMENTATION)

#if !defined(AUTOBOOST_ASIO_WINDOWS) && !defined(__CYGWIN__)
# include <termios.h>
#endif // !defined(AUTOBOOST_ASIO_WINDOWS) && !defined(__CYGWIN__)

#include <autoboost/asio/detail/socket_types.hpp>
#include <autoboost/system/error_code.hpp>

#if defined(GENERATING_DOCUMENTATION)
# define AUTOBOOST_ASIO_OPTION_STORAGE implementation_defined
#elif defined(AUTOBOOST_ASIO_WINDOWS) || defined(__CYGWIN__)
# define AUTOBOOST_ASIO_OPTION_STORAGE DCB
#else
# define AUTOBOOST_ASIO_OPTION_STORAGE termios
#endif

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {

/// The serial_port_base class is used as a base for the basic_serial_port class
/// template so that we have a common place to define the serial port options.
class serial_port_base
{
public:
  /// Serial port option to permit changing the baud rate.
  /**
   * Implements changing the baud rate for a given serial port.
   */
  class baud_rate
  {
  public:
    explicit baud_rate(unsigned int rate = 0);
    unsigned int value() const;
    AUTOBOOST_ASIO_DECL autoboost::system::error_code store(
        AUTOBOOST_ASIO_OPTION_STORAGE& storage,
        autoboost::system::error_code& ec) const;
    AUTOBOOST_ASIO_DECL autoboost::system::error_code load(
        const AUTOBOOST_ASIO_OPTION_STORAGE& storage,
        autoboost::system::error_code& ec);
  private:
    unsigned int value_;
  };

  /// Serial port option to permit changing the flow control.
  /**
   * Implements changing the flow control for a given serial port.
   */
  class flow_control
  {
  public:
    enum type { none, software, hardware };
    AUTOBOOST_ASIO_DECL explicit flow_control(type t = none);
    type value() const;
    AUTOBOOST_ASIO_DECL autoboost::system::error_code store(
        AUTOBOOST_ASIO_OPTION_STORAGE& storage,
        autoboost::system::error_code& ec) const;
    AUTOBOOST_ASIO_DECL autoboost::system::error_code load(
        const AUTOBOOST_ASIO_OPTION_STORAGE& storage,
        autoboost::system::error_code& ec);
  private:
    type value_;
  };

  /// Serial port option to permit changing the parity.
  /**
   * Implements changing the parity for a given serial port.
   */
  class parity
  {
  public:
    enum type { none, odd, even };
    AUTOBOOST_ASIO_DECL explicit parity(type t = none);
    type value() const;
    AUTOBOOST_ASIO_DECL autoboost::system::error_code store(
        AUTOBOOST_ASIO_OPTION_STORAGE& storage,
        autoboost::system::error_code& ec) const;
    AUTOBOOST_ASIO_DECL autoboost::system::error_code load(
        const AUTOBOOST_ASIO_OPTION_STORAGE& storage,
        autoboost::system::error_code& ec);
  private:
    type value_;
  };

  /// Serial port option to permit changing the number of stop bits.
  /**
   * Implements changing the number of stop bits for a given serial port.
   */
  class stop_bits
  {
  public:
    enum type { one, onepointfive, two };
    AUTOBOOST_ASIO_DECL explicit stop_bits(type t = one);
    type value() const;
    AUTOBOOST_ASIO_DECL autoboost::system::error_code store(
        AUTOBOOST_ASIO_OPTION_STORAGE& storage,
        autoboost::system::error_code& ec) const;
    AUTOBOOST_ASIO_DECL autoboost::system::error_code load(
        const AUTOBOOST_ASIO_OPTION_STORAGE& storage,
        autoboost::system::error_code& ec);
  private:
    type value_;
  };

  /// Serial port option to permit changing the character size.
  /**
   * Implements changing the character size for a given serial port.
   */
  class character_size
  {
  public:
    AUTOBOOST_ASIO_DECL explicit character_size(unsigned int t = 8);
    unsigned int value() const;
    AUTOBOOST_ASIO_DECL autoboost::system::error_code store(
        AUTOBOOST_ASIO_OPTION_STORAGE& storage,
        autoboost::system::error_code& ec) const;
    AUTOBOOST_ASIO_DECL autoboost::system::error_code load(
        const AUTOBOOST_ASIO_OPTION_STORAGE& storage,
        autoboost::system::error_code& ec);
  private:
    unsigned int value_;
  };

protected:
  /// Protected destructor to prevent deletion through this type.
  ~serial_port_base()
  {
  }
};

} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#undef AUTOBOOST_ASIO_OPTION_STORAGE

#include <autoboost/asio/impl/serial_port_base.hpp>
#if defined(AUTOBOOST_ASIO_HEADER_ONLY)
# include <autoboost/asio/impl/serial_port_base.ipp>
#endif // defined(AUTOBOOST_ASIO_HEADER_ONLY)

#endif // defined(AUTOBOOST_ASIO_HAS_SERIAL_PORT)
       //   || defined(GENERATING_DOCUMENTATION)

#endif // AUTOBOOST_ASIO_SERIAL_PORT_BASE_HPP
