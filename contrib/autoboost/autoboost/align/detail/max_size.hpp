/*
(c) 2014-2015 Glen Joseph Fernandes
<glenjofe -at- gmail.com>

Distributed under the Boost Software
License, Version 1.0.
http://autoboost.org/LICENSE_1_0.txt
*/
#ifndef AUTOBOOST_ALIGN_DETAIL_MAX_SIZE_HPP
#define AUTOBOOST_ALIGN_DETAIL_MAX_SIZE_HPP

#include <autoboost/align/detail/integral_constant.hpp>
#include <cstddef>

namespace autoboost {
namespace alignment {
namespace detail {

template<std::size_t A, std::size_t B>
struct max_size
    : integral_constant<std::size_t, (A > B) ? A : B> {
};

} /* .detail */
} /* .alignment */
} /* .boost */

#endif
