//
// detail/cstdint.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_CSTDINT_HPP
#define AUTOBOOST_ASIO_DETAIL_CSTDINT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_HAS_CSTDINT)
# include <cstdint>
#else // defined(AUTOBOOST_ASIO_HAS_CSTDINT)
# include <autoboost/cstdint.hpp>
#endif // defined(AUTOBOOST_ASIO_HAS_CSTDINT)

namespace autoboost {
namespace asio {

#if defined(AUTOBOOST_ASIO_HAS_CSTDINT)
using std::int16_t;
using std::uint16_t;
using std::int32_t;
using std::uint32_t;
using std::int64_t;
using std::uint64_t;
#else // defined(AUTOBOOST_ASIO_HAS_CSTDINT)
using autoboost::int16_t;
using autoboost::uint16_t;
using autoboost::int32_t;
using autoboost::uint32_t;
using autoboost::int64_t;
using autoboost::uint64_t;
#endif // defined(AUTOBOOST_ASIO_HAS_CSTDINT)

} // namespace asio
} // namespace autoboost

#endif // AUTOBOOST_ASIO_DETAIL_CSTDINT_HPP
