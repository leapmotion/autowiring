//
// detail/assert.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_ASSERT_HPP
#define AUTOBOOST_ASIO_DETAIL_ASSERT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_ASSERT)
# include <autoboost/assert.hpp>
#else // defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_ASSERT)
# include <cassert>
#endif // defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_ASSERT)

#if defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_ASSERT)
# define AUTOBOOST_ASIO_ASSERT(expr) AUTOBOOST_ASSERT(expr)
#else // defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_ASSERT)
# define AUTOBOOST_ASIO_ASSERT(expr) assert(expr)
#endif // defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_ASSERT)

#endif // AUTOBOOST_ASIO_DETAIL_ASSERT_HPP
