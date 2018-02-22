/*
Copyright 2014-2015 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AUTOBOOST_ALIGN_DETAIL_MAX_ALIGN_HPP
#define AUTOBOOST_ALIGN_DETAIL_MAX_ALIGN_HPP

#include <autoboost/align/detail/max_size.hpp>
#include <autoboost/align/alignment_of.hpp>

namespace autoboost {
namespace alignment {
namespace detail {

template<class A, class B>
struct max_align
    : max_size<alignment_of<A>::value, alignment_of<B>::value> { };

} /* detail */
} /* alignment */
} /* autoboost */

#endif
