//
// error.hpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_ERROR_HPP
#define AUTOBOOST_ASIO_ERROR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>
#include <autoboost/cerrno.hpp>
#include <autoboost/system/error_code.hpp>
#include <autoboost/system/system_error.hpp>
#if defined(AUTOBOOST_ASIO_WINDOWS) \
  || defined(__CYGWIN__) \
  || defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)
# include <winerror.h>
#else
# include <cerrno>
# include <netdb.h>
#endif

#if defined(GENERATING_DOCUMENTATION)
/// INTERNAL ONLY.
# define AUTOBOOST_ASIO_NATIVE_ERROR(e) implementation_defined
/// INTERNAL ONLY.
# define AUTOBOOST_ASIO_SOCKET_ERROR(e) implementation_defined
/// INTERNAL ONLY.
# define AUTOBOOST_ASIO_NETDB_ERROR(e) implementation_defined
/// INTERNAL ONLY.
# define AUTOBOOST_ASIO_GETADDRINFO_ERROR(e) implementation_defined
/// INTERNAL ONLY.
# define AUTOBOOST_ASIO_WIN_OR_POSIX(e_win, e_posix) implementation_defined
#elif defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)
# define AUTOBOOST_ASIO_NATIVE_ERROR(e) __HRESULT_FROM_WIN32(e)
# define AUTOBOOST_ASIO_SOCKET_ERROR(e) __HRESULT_FROM_WIN32(WSA ## e)
# define AUTOBOOST_ASIO_NETDB_ERROR(e) __HRESULT_FROM_WIN32(WSA ## e)
# define AUTOBOOST_ASIO_GETADDRINFO_ERROR(e) __HRESULT_FROM_WIN32(WSA ## e)
# define AUTOBOOST_ASIO_WIN_OR_POSIX(e_win, e_posix) e_win
#elif defined(AUTOBOOST_ASIO_WINDOWS) || defined(__CYGWIN__)
# define AUTOBOOST_ASIO_NATIVE_ERROR(e) e
# define AUTOBOOST_ASIO_SOCKET_ERROR(e) WSA ## e
# define AUTOBOOST_ASIO_NETDB_ERROR(e) WSA ## e
# define AUTOBOOST_ASIO_GETADDRINFO_ERROR(e) WSA ## e
# define AUTOBOOST_ASIO_WIN_OR_POSIX(e_win, e_posix) e_win
#else
# define AUTOBOOST_ASIO_NATIVE_ERROR(e) e
# define AUTOBOOST_ASIO_SOCKET_ERROR(e) e
# define AUTOBOOST_ASIO_NETDB_ERROR(e) e
# define AUTOBOOST_ASIO_GETADDRINFO_ERROR(e) e
# define AUTOBOOST_ASIO_WIN_OR_POSIX(e_win, e_posix) e_posix
#endif

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace error {

enum basic_errors
{
  /// Permission denied.
  access_denied = AUTOBOOST_ASIO_SOCKET_ERROR(EACCES),

  /// Address family not supported by protocol.
  address_family_not_supported = AUTOBOOST_ASIO_SOCKET_ERROR(EAFNOSUPPORT),

  /// Address already in use.
  address_in_use = AUTOBOOST_ASIO_SOCKET_ERROR(EADDRINUSE),

  /// Transport endpoint is already connected.
  already_connected = AUTOBOOST_ASIO_SOCKET_ERROR(EISCONN),

  /// Operation already in progress.
  already_started = AUTOBOOST_ASIO_SOCKET_ERROR(EALREADY),

  /// Broken pipe.
  broken_pipe = AUTOBOOST_ASIO_WIN_OR_POSIX(
      AUTOBOOST_ASIO_NATIVE_ERROR(ERROR_BROKEN_PIPE),
      AUTOBOOST_ASIO_NATIVE_ERROR(EPIPE)),

  /// A connection has been aborted.
  connection_aborted = AUTOBOOST_ASIO_SOCKET_ERROR(ECONNABORTED),

  /// Connection refused.
  connection_refused = AUTOBOOST_ASIO_SOCKET_ERROR(ECONNREFUSED),

  /// Connection reset by peer.
  connection_reset = AUTOBOOST_ASIO_SOCKET_ERROR(ECONNRESET),

  /// Bad file descriptor.
  bad_descriptor = AUTOBOOST_ASIO_SOCKET_ERROR(EBADF),

  /// Bad address.
  fault = AUTOBOOST_ASIO_SOCKET_ERROR(EFAULT),

  /// No route to host.
  host_unreachable = AUTOBOOST_ASIO_SOCKET_ERROR(EHOSTUNREACH),

  /// Operation now in progress.
  in_progress = AUTOBOOST_ASIO_SOCKET_ERROR(EINPROGRESS),

  /// Interrupted system call.
  interrupted = AUTOBOOST_ASIO_SOCKET_ERROR(EINTR),

  /// Invalid argument.
  invalid_argument = AUTOBOOST_ASIO_SOCKET_ERROR(EINVAL),

  /// Message too long.
  message_size = AUTOBOOST_ASIO_SOCKET_ERROR(EMSGSIZE),

  /// The name was too long.
  name_too_long = AUTOBOOST_ASIO_SOCKET_ERROR(ENAMETOOLONG),

  /// Network is down.
  network_down = AUTOBOOST_ASIO_SOCKET_ERROR(ENETDOWN),

  /// Network dropped connection on reset.
  network_reset = AUTOBOOST_ASIO_SOCKET_ERROR(ENETRESET),

  /// Network is unreachable.
  network_unreachable = AUTOBOOST_ASIO_SOCKET_ERROR(ENETUNREACH),

  /// Too many open files.
  no_descriptors = AUTOBOOST_ASIO_SOCKET_ERROR(EMFILE),

  /// No buffer space available.
  no_buffer_space = AUTOBOOST_ASIO_SOCKET_ERROR(ENOBUFS),

  /// Cannot allocate memory.
  no_memory = AUTOBOOST_ASIO_WIN_OR_POSIX(
      AUTOBOOST_ASIO_NATIVE_ERROR(ERROR_OUTOFMEMORY),
      AUTOBOOST_ASIO_NATIVE_ERROR(ENOMEM)),

  /// Operation not permitted.
  no_permission = AUTOBOOST_ASIO_WIN_OR_POSIX(
      AUTOBOOST_ASIO_NATIVE_ERROR(ERROR_ACCESS_DENIED),
      AUTOBOOST_ASIO_NATIVE_ERROR(EPERM)),

  /// Protocol not available.
  no_protocol_option = AUTOBOOST_ASIO_SOCKET_ERROR(ENOPROTOOPT),

  /// No such device.
  no_such_device = AUTOBOOST_ASIO_WIN_OR_POSIX(
      AUTOBOOST_ASIO_NATIVE_ERROR(ERROR_BAD_UNIT),
      AUTOBOOST_ASIO_NATIVE_ERROR(ENODEV)),

  /// Transport endpoint is not connected.
  not_connected = AUTOBOOST_ASIO_SOCKET_ERROR(ENOTCONN),

  /// Socket operation on non-socket.
  not_socket = AUTOBOOST_ASIO_SOCKET_ERROR(ENOTSOCK),

  /// Operation cancelled.
  operation_aborted = AUTOBOOST_ASIO_WIN_OR_POSIX(
      AUTOBOOST_ASIO_NATIVE_ERROR(ERROR_OPERATION_ABORTED),
      AUTOBOOST_ASIO_NATIVE_ERROR(ECANCELED)),

  /// Operation not supported.
  operation_not_supported = AUTOBOOST_ASIO_SOCKET_ERROR(EOPNOTSUPP),

  /// Cannot send after transport endpoint shutdown.
  shut_down = AUTOBOOST_ASIO_SOCKET_ERROR(ESHUTDOWN),

  /// Connection timed out.
  timed_out = AUTOBOOST_ASIO_SOCKET_ERROR(ETIMEDOUT),

  /// Resource temporarily unavailable.
  try_again = AUTOBOOST_ASIO_WIN_OR_POSIX(
      AUTOBOOST_ASIO_NATIVE_ERROR(ERROR_RETRY),
      AUTOBOOST_ASIO_NATIVE_ERROR(EAGAIN)),

  /// The socket is marked non-blocking and the requested operation would block.
  would_block = AUTOBOOST_ASIO_SOCKET_ERROR(EWOULDBLOCK)
};

enum netdb_errors
{
  /// Host not found (authoritative).
  host_not_found = AUTOBOOST_ASIO_NETDB_ERROR(HOST_NOT_FOUND),

  /// Host not found (non-authoritative).
  host_not_found_try_again = AUTOBOOST_ASIO_NETDB_ERROR(TRY_AGAIN),

  /// The query is valid but does not have associated address data.
  no_data = AUTOBOOST_ASIO_NETDB_ERROR(NO_DATA),

  /// A non-recoverable error occurred.
  no_recovery = AUTOBOOST_ASIO_NETDB_ERROR(NO_RECOVERY)
};

enum addrinfo_errors
{
  /// The service is not supported for the given socket type.
  service_not_found = AUTOBOOST_ASIO_WIN_OR_POSIX(
      AUTOBOOST_ASIO_NATIVE_ERROR(WSATYPE_NOT_FOUND),
      AUTOBOOST_ASIO_GETADDRINFO_ERROR(EAI_SERVICE)),

  /// The socket type is not supported.
  socket_type_not_supported = AUTOBOOST_ASIO_WIN_OR_POSIX(
      AUTOBOOST_ASIO_NATIVE_ERROR(WSAESOCKTNOSUPPORT),
      AUTOBOOST_ASIO_GETADDRINFO_ERROR(EAI_SOCKTYPE))
};

enum misc_errors
{
  /// Already open.
  already_open = 1,

  /// End of file or stream.
  eof,

  /// Element not found.
  not_found,

  /// The descriptor cannot fit into the select system call's fd_set.
  fd_set_failure
};

inline const autoboost::system::error_category& get_system_category()
{
  return autoboost::system::system_category();
}

#if !defined(AUTOBOOST_ASIO_WINDOWS) && !defined(__CYGWIN__)

extern AUTOBOOST_ASIO_DECL
const autoboost::system::error_category& get_netdb_category();

extern AUTOBOOST_ASIO_DECL
const autoboost::system::error_category& get_addrinfo_category();

#else // !defined(AUTOBOOST_ASIO_WINDOWS) && !defined(__CYGWIN__)

inline const autoboost::system::error_category& get_netdb_category()
{
  return get_system_category();
}

inline const autoboost::system::error_category& get_addrinfo_category()
{
  return get_system_category();
}

#endif // !defined(AUTOBOOST_ASIO_WINDOWS) && !defined(__CYGWIN__)

extern AUTOBOOST_ASIO_DECL
const autoboost::system::error_category& get_misc_category();

static const autoboost::system::error_category& system_category
  = autoboost::asio::error::get_system_category();
static const autoboost::system::error_category& netdb_category
  = autoboost::asio::error::get_netdb_category();
static const autoboost::system::error_category& addrinfo_category
  = autoboost::asio::error::get_addrinfo_category();
static const autoboost::system::error_category& misc_category
  = autoboost::asio::error::get_misc_category();

} // namespace error
} // namespace asio
} // namespace autoboost

namespace autoboost {
namespace system {

template<> struct is_error_code_enum<autoboost::asio::error::basic_errors>
{
  static const bool value = true;
};

template<> struct is_error_code_enum<autoboost::asio::error::netdb_errors>
{
  static const bool value = true;
};

template<> struct is_error_code_enum<autoboost::asio::error::addrinfo_errors>
{
  static const bool value = true;
};

template<> struct is_error_code_enum<autoboost::asio::error::misc_errors>
{
  static const bool value = true;
};

} // namespace system
} // namespace autoboost

namespace autoboost {
namespace asio {
namespace error {

inline autoboost::system::error_code make_error_code(basic_errors e)
{
  return autoboost::system::error_code(
      static_cast<int>(e), get_system_category());
}

inline autoboost::system::error_code make_error_code(netdb_errors e)
{
  return autoboost::system::error_code(
      static_cast<int>(e), get_netdb_category());
}

inline autoboost::system::error_code make_error_code(addrinfo_errors e)
{
  return autoboost::system::error_code(
      static_cast<int>(e), get_addrinfo_category());
}

inline autoboost::system::error_code make_error_code(misc_errors e)
{
  return autoboost::system::error_code(
      static_cast<int>(e), get_misc_category());
}

} // namespace error
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#undef AUTOBOOST_ASIO_NATIVE_ERROR
#undef AUTOBOOST_ASIO_SOCKET_ERROR
#undef AUTOBOOST_ASIO_NETDB_ERROR
#undef AUTOBOOST_ASIO_GETADDRINFO_ERROR
#undef AUTOBOOST_ASIO_WIN_OR_POSIX

#if defined(AUTOBOOST_ASIO_HEADER_ONLY)
# include <autoboost/asio/impl/error.ipp>
#endif // defined(AUTOBOOST_ASIO_HEADER_ONLY)

#endif // AUTOBOOST_ASIO_ERROR_HPP
