//
// detail/regex_fwd.hpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_REGEX_FWD_HPP
#define AUTOBOOST_ASIO_DETAIL_REGEX_FWD_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#if defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_REGEX)

#include <autoboost/regex_fwd.hpp>
#include <autoboost/regex/v4/match_flags.hpp>

namespace autoboost {

template <class BidiIterator>
struct sub_match;

template <class BidiIterator, class Allocator>
class match_results;

} // namespace autoboost

#endif // defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_REGEX)

#endif // AUTOBOOST_ASIO_DETAIL_REGEX_FWD_HPP
