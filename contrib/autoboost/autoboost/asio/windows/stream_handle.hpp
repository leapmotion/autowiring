//
// windows/stream_handle.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_WINDOWS_STREAM_HANDLE_HPP
#define AUTOBOOST_ASIO_WINDOWS_STREAM_HANDLE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_HAS_WINDOWS_STREAM_HANDLE) \
  || defined(GENERATING_DOCUMENTATION)

#include <autoboost/asio/windows/basic_stream_handle.hpp>

namespace autoboost {
namespace asio {
namespace windows {

/// Typedef for the typical usage of a stream-oriented handle.
typedef basic_stream_handle<> stream_handle;

} // namespace windows
} // namespace asio
} // namespace autoboost

#endif // defined(AUTOBOOST_ASIO_HAS_WINDOWS_STREAM_HANDLE)
       //   || defined(GENERATING_DOCUMENTATION)

#endif // AUTOBOOST_ASIO_WINDOWS_STREAM_HANDLE_HPP
