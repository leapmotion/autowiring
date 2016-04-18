//
// detail/addressof.hpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_ADDRESSOF_HPP
#define AUTOBOOST_ASIO_DETAIL_ADDRESSOF_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_HAS_STD_ADDRESSOF)
# include <memory>
#else // defined(AUTOBOOST_ASIO_HAS_STD_ADDRESSOF)
# include <autoboost/utility/addressof.hpp>
#endif // defined(AUTOBOOST_ASIO_HAS_STD_ADDRESSOF)

namespace autoboost {
namespace asio {
namespace detail {

#if defined(AUTOBOOST_ASIO_HAS_STD_ADDRESSOF)
using std::addressof;
#else // defined(AUTOBOOST_ASIO_HAS_STD_ADDRESSOF)
using autoboost::addressof;
#endif // defined(AUTOBOOST_ASIO_HAS_STD_ADDRESSOF)

} // namespace detail
} // namespace asio
} // namespace autoboost

#endif // AUTOBOOST_ASIO_DETAIL_ADDRESSOF_HPP
