/*
(c) 2014 Glen Joseph Fernandes
<glenjofe -at- gmail.com>

Distributed under the Boost Software
License, Version 1.0.
http://autoboost.org/LICENSE_1_0.txt
*/
#ifndef AUTOBOOST_ALIGN_DETAIL_MAX_OBJECTS_HPP
#define AUTOBOOST_ALIGN_DETAIL_MAX_OBJECTS_HPP

#include <autoboost/align/detail/integral_constant.hpp>
#include <cstddef>

namespace autoboost {
namespace alignment {
namespace detail {

template<class T>
struct max_objects
    : integral_constant<std::size_t,
        ~static_cast<std::size_t>(0) / sizeof(T)> {
};

} /* .detail */
} /* .alignment */
} /* .boost */

#endif
