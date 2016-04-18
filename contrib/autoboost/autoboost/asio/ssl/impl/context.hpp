//
// ssl/impl/context.hpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2005 Voipster / Indrek dot Juhani at voipster dot com
// Copyright (c) 2005-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_SSL_IMPL_CONTEXT_HPP
#define AUTOBOOST_ASIO_SSL_IMPL_CONTEXT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if !defined(AUTOBOOST_ASIO_ENABLE_OLD_SSL)
# include <autoboost/asio/detail/throw_error.hpp>
#endif // !defined(AUTOBOOST_ASIO_ENABLE_OLD_SSL)

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace ssl {

#if !defined(AUTOBOOST_ASIO_ENABLE_OLD_SSL)

template <typename VerifyCallback>
void context::set_verify_callback(VerifyCallback callback)
{
  autoboost::system::error_code ec;
  this->set_verify_callback(callback, ec);
  autoboost::asio::detail::throw_error(ec, "set_verify_callback");
}

template <typename VerifyCallback>
autoboost::system::error_code context::set_verify_callback(
    VerifyCallback callback, autoboost::system::error_code& ec)
{
  return do_set_verify_callback(
      new detail::verify_callback<VerifyCallback>(callback), ec);
}

template <typename PasswordCallback>
void context::set_password_callback(PasswordCallback callback)
{
  autoboost::system::error_code ec;
  this->set_password_callback(callback, ec);
  autoboost::asio::detail::throw_error(ec, "set_password_callback");
}

template <typename PasswordCallback>
autoboost::system::error_code context::set_password_callback(
    PasswordCallback callback, autoboost::system::error_code& ec)
{
  return do_set_password_callback(
      new detail::password_callback<PasswordCallback>(callback), ec);
}

#endif // !defined(AUTOBOOST_ASIO_ENABLE_OLD_SSL)

} // namespace ssl
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#endif // AUTOBOOST_ASIO_SSL_IMPL_CONTEXT_HPP
