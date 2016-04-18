//
// detail/weak_ptr.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_WEAK_PTR_HPP
#define AUTOBOOST_ASIO_DETAIL_WEAK_PTR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_HAS_STD_SHARED_PTR)
# include <memory>
#else // defined(AUTOBOOST_ASIO_HAS_STD_SHARED_PTR)
# include <autoboost/weak_ptr.hpp>
#endif // defined(AUTOBOOST_ASIO_HAS_STD_SHARED_PTR)

namespace autoboost {
namespace asio {
namespace detail {

#if defined(AUTOBOOST_ASIO_HAS_STD_SHARED_PTR)
using std::weak_ptr;
#else // defined(AUTOBOOST_ASIO_HAS_STD_SHARED_PTR)
using autoboost::weak_ptr;
#endif // defined(AUTOBOOST_ASIO_HAS_STD_SHARED_PTR)

} // namespace detail
} // namespace asio
} // namespace autoboost

#endif // AUTOBOOST_ASIO_DETAIL_WEAK_PTR_HPP
