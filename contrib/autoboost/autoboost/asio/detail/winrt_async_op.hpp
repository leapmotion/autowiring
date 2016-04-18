//
// detail/winrt_async_op.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_WINRT_ASYNC_OP_HPP
#define AUTOBOOST_ASIO_DETAIL_WINRT_ASYNC_OP_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>
#include <autoboost/asio/detail/operation.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace detail {

template <typename TResult>
class winrt_async_op
  : public operation
{
public:
  // The error code to be passed to the completion handler.
  autoboost::system::error_code ec_;

  // The result of the operation, to be passed to the completion handler.
  TResult result_;

protected:
  winrt_async_op(func_type complete_func)
    : operation(complete_func),
      result_()
  {
  }
};

template <>
class winrt_async_op<void>
  : public operation
{
public:
  // The error code to be passed to the completion handler.
  autoboost::system::error_code ec_;

protected:
  winrt_async_op(func_type complete_func)
    : operation(complete_func)
  {
  }
};

} // namespace detail
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#endif // AUTOBOOST_ASIO_DETAIL_WINRT_ASYNC_OP_HPP
