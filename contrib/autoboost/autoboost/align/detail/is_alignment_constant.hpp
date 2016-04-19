/*
(c) 2014 Glen Joseph Fernandes
<glenjofe -at- gmail.com>

Distributed under the Boost Software
License, Version 1.0.
http://autoboost.org/LICENSE_1_0.txt
*/
#ifndef AUTOBOOST_ALIGN_DETAIL_IS_ALIGNMENT_CONSTANT_HPP
#define AUTOBOOST_ALIGN_DETAIL_IS_ALIGNMENT_CONSTANT_HPP

#include <autoboost/align/detail/integral_constant.hpp>
#include <cstddef>

namespace autoboost {
namespace alignment {
namespace detail {

template<std::size_t N>
struct is_alignment_constant
    : integral_constant<bool, (N > 0) && ((N & (N - 1)) == 0)> {
};

} /* .detail */
} /* .alignment */
} /* .boost */

#endif
