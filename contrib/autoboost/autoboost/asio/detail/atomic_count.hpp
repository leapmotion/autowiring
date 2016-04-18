//
// detail/atomic_count.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_ATOMIC_COUNT_HPP
#define AUTOBOOST_ASIO_DETAIL_ATOMIC_COUNT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if !defined(AUTOBOOST_ASIO_HAS_THREADS)
// Nothing to include.
#elif defined(AUTOBOOST_ASIO_HAS_STD_ATOMIC)
# include <atomic>
#else // defined(AUTOBOOST_ASIO_HAS_STD_ATOMIC)
# include <autoboost/detail/atomic_count.hpp>
#endif // defined(AUTOBOOST_ASIO_HAS_STD_ATOMIC)

namespace autoboost {
namespace asio {
namespace detail {

#if !defined(AUTOBOOST_ASIO_HAS_THREADS)
typedef long atomic_count;
inline void increment(atomic_count& a, long b) { a += b; }
#elif defined(AUTOBOOST_ASIO_HAS_STD_ATOMIC)
typedef std::atomic<long> atomic_count;
inline void increment(atomic_count& a, long b) { a += b; }
#else // defined(AUTOBOOST_ASIO_HAS_STD_ATOMIC)
typedef autoboost::detail::atomic_count atomic_count;
inline void increment(atomic_count& a, long b) { while (b > 0) ++a, --b; }
#endif // defined(AUTOBOOST_ASIO_HAS_STD_ATOMIC)

} // namespace detail
} // namespace asio
} // namespace autoboost

#endif // AUTOBOOST_ASIO_DETAIL_ATOMIC_COUNT_HPP
