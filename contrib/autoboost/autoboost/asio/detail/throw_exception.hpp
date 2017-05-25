//
// detail/throw_exception.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_THROW_EXCEPTION_HPP
#define AUTOBOOST_ASIO_DETAIL_THROW_EXCEPTION_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_THROW_EXCEPTION)
# include <autoboost/throw_exception.hpp>
#endif // defined(AUTOBOOST_ASIO_AUTOBOOST_THROW_EXCEPTION)

namespace autoboost {
namespace asio {
namespace detail {

#if defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_THROW_EXCEPTION)
using autoboost::throw_exception;
#else // defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_THROW_EXCEPTION)

// Declare the throw_exception function for all targets.
template <typename Exception>
void throw_exception(const Exception& e);

// Only define the throw_exception function when exceptions are enabled.
// Otherwise, it is up to the application to provide a definition of this
// function.
# if !defined(AUTOBOOST_ASIO_NO_EXCEPTIONS)
template <typename Exception>
void throw_exception(const Exception& e)
{
  throw e;
}
# endif // !defined(AUTOBOOST_ASIO_NO_EXCEPTIONS)

#endif // defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_THROW_EXCEPTION)

} // namespace detail
} // namespace asio
} // namespace autoboost

#endif // AUTOBOOST_ASIO_DETAIL_THROW_EXCEPTION_HPP
