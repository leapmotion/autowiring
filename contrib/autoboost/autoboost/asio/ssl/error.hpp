//
// ssl/error.hpp
// ~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_SSL_ERROR_HPP
#define AUTOBOOST_ASIO_SSL_ERROR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>
#include <autoboost/system/error_code.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace error {

enum ssl_errors
{
};

extern AUTOBOOST_ASIO_DECL
const autoboost::system::error_category& get_ssl_category();

static const autoboost::system::error_category& ssl_category
  = autoboost::asio::error::get_ssl_category();

} // namespace error
} // namespace asio
} // namespace autoboost

namespace autoboost {
namespace system {

template<> struct is_error_code_enum<autoboost::asio::error::ssl_errors>
{
  static const bool value = true;
};

} // namespace system
} // namespace autoboost

namespace autoboost {
namespace asio {
namespace error {

inline autoboost::system::error_code make_error_code(ssl_errors e)
{
  return autoboost::system::error_code(
      static_cast<int>(e), get_ssl_category());
}

} // namespace error
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#if defined(AUTOBOOST_ASIO_HEADER_ONLY)
# include <autoboost/asio/ssl/impl/error.ipp>
#endif // defined(AUTOBOOST_ASIO_HEADER_ONLY)

#endif // AUTOBOOST_ASIO_SSL_ERROR_HPP
