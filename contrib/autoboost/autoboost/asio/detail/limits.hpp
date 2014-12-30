//
// detail/limits.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_LIMITS_HPP
#define AUTOBOOST_ASIO_DETAIL_LIMITS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_LIMITS)
# include <autoboost/limits.hpp>
#else // defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_LIMITS)
# include <limits>
#endif // defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_LIMITS)

#endif // AUTOBOOST_ASIO_DETAIL_LIMITS_HPP
