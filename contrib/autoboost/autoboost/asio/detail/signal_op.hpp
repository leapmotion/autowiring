//
// detail/signal_op.hpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_SIGNAL_OP_HPP
#define AUTOBOOST_ASIO_DETAIL_SIGNAL_OP_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>
#include <autoboost/asio/detail/operation.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace detail {

class signal_op
  : public operation
{
public:
  // The error code to be passed to the completion handler.
  autoboost::system::error_code ec_;

  // The signal number to be passed to the completion handler.
  int signal_number_;

protected:
  signal_op(func_type func)
    : operation(func),
      signal_number_(0)
  {
  }
};

} // namespace detail
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#endif // AUTOBOOST_ASIO_DETAIL_SIGNAL_OP_HPP
