//
// detail/throw_error.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_THROW_ERROR_HPP
#define AUTOBOOST_ASIO_DETAIL_THROW_ERROR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>
#include <autoboost/system/error_code.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace detail {

AUTOBOOST_ASIO_DECL void do_throw_error(const autoboost::system::error_code& err);

AUTOBOOST_ASIO_DECL void do_throw_error(const autoboost::system::error_code& err,
    const char* location);

inline void throw_error(const autoboost::system::error_code& err)
{
  if (err)
    do_throw_error(err);
}

inline void throw_error(const autoboost::system::error_code& err,
    const char* location)
{
  if (err)
    do_throw_error(err, location);
}

} // namespace detail
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#if defined(AUTOBOOST_ASIO_HEADER_ONLY)
# include <autoboost/asio/detail/impl/throw_error.ipp>
#endif // defined(AUTOBOOST_ASIO_HEADER_ONLY)

#endif // AUTOBOOST_ASIO_DETAIL_THROW_ERROR_HPP
