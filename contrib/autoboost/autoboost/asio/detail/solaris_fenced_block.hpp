//
// detail/solaris_fenced_block.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_SOLARIS_FENCED_BLOCK_HPP
#define AUTOBOOST_ASIO_DETAIL_SOLARIS_FENCED_BLOCK_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(__sun)

#include <atomic.h>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace detail {

class solaris_fenced_block
  : private noncopyable
{
public:
  enum half_t { half };
  enum full_t { full };

  // Constructor for a half fenced block.
  explicit solaris_fenced_block(half_t)
  {
  }

  // Constructor for a full fenced block.
  explicit solaris_fenced_block(full_t)
  {
    membar_consumer();
  }

  // Destructor.
  ~solaris_fenced_block()
  {
    membar_producer();
  }
};

} // namespace detail
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#endif // defined(__sun)

#endif // AUTOBOOST_ASIO_DETAIL_SOLARIS_FENCED_BLOCK_HPP
