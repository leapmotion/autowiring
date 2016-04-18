//
// detail/impl/throw_error.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_IMPL_THROW_ERROR_IPP
#define AUTOBOOST_ASIO_DETAIL_IMPL_THROW_ERROR_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>
#include <autoboost/asio/detail/throw_error.hpp>
#include <autoboost/asio/detail/throw_exception.hpp>
#include <autoboost/system/system_error.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace detail {

void do_throw_error(const autoboost::system::error_code& err)
{
  autoboost::system::system_error e(err);
  autoboost::asio::detail::throw_exception(e);
}

void do_throw_error(const autoboost::system::error_code& err, const char* location)
{
  autoboost::system::system_error e(err, location);
  autoboost::asio::detail::throw_exception(e);
}

} // namespace detail
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#endif // AUTOBOOST_ASIO_DETAIL_IMPL_THROW_ERROR_IPP
