//
// streambuf.hpp
// ~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_STREAMBUF_HPP
#define AUTOBOOST_ASIO_STREAMBUF_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if !defined(AUTOBOOST_ASIO_NO_IOSTREAM)

#include <autoboost/asio/basic_streambuf.hpp>

namespace autoboost {
namespace asio {

/// Typedef for the typical usage of basic_streambuf.
typedef basic_streambuf<> streambuf;

} // namespace asio
} // namespace autoboost

#endif // !defined(AUTOBOOST_ASIO_NO_IOSTREAM)

#endif // AUTOBOOST_ASIO_STREAMBUF_HPP
