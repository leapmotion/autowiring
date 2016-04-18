//
// signal_set.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_SIGNAL_SET_HPP
#define AUTOBOOST_ASIO_SIGNAL_SET_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>
#include <autoboost/asio/basic_signal_set.hpp>

namespace autoboost {
namespace asio {

/// Typedef for the typical usage of a signal set.
typedef basic_signal_set<> signal_set;

} // namespace asio
} // namespace autoboost

#endif // AUTOBOOST_ASIO_SIGNAL_SET_HPP
