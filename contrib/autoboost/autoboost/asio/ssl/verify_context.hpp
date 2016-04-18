//
// ssl/verify_context.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_SSL_VERIFY_CONTEXT_HPP
#define AUTOBOOST_ASIO_SSL_VERIFY_CONTEXT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if !defined(AUTOBOOST_ASIO_ENABLE_OLD_SSL)
# include <autoboost/asio/detail/noncopyable.hpp>
# include <autoboost/asio/ssl/detail/openssl_types.hpp>
#endif // !defined(AUTOBOOST_ASIO_ENABLE_OLD_SSL)

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace ssl {

#if !defined(AUTOBOOST_ASIO_ENABLE_OLD_SSL)

/// A simple wrapper around the X509_STORE_CTX type, used during verification of
/// a peer certificate.
/**
 * @note The verify_context does not own the underlying X509_STORE_CTX object.
 */
class verify_context
  : private noncopyable
{
public:
  /// The native handle type of the verification context.
  typedef X509_STORE_CTX* native_handle_type;

  /// Constructor.
  explicit verify_context(native_handle_type handle)
    : handle_(handle)
  {
  }

  /// Get the underlying implementation in the native type.
  /**
   * This function may be used to obtain the underlying implementation of the
   * context. This is intended to allow access to context functionality that is
   * not otherwise provided.
   */
  native_handle_type native_handle()
  {
    return handle_;
  }

private:
  // The underlying native implementation.
  native_handle_type handle_;
};

#endif // defined(AUTOBOOST_ASIO_ENABLE_OLD_SSL)

} // namespace ssl
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#endif // AUTOBOOST_ASIO_SSL_VERIFY_CONTEXT_HPP
