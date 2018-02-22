/*
Copyright 2014-2015 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AUTOBOOST_ALIGN_DETAIL_ALIGNMENT_OF_MSVC_HPP
#define AUTOBOOST_ALIGN_DETAIL_ALIGNMENT_OF_MSVC_HPP

#include <autoboost/align/detail/min_size.hpp>

namespace autoboost {
namespace alignment {
namespace detail {

template<class T>
struct offset_value {
    T first;
    char value;
    T second;
};

template<class T>
struct alignment_of
    : min_size<sizeof(T),
        sizeof(offset_value<T>) - (sizeof(T) << 1)> { };

} /* detail */
} /* alignment */
} /* autoboost */

#endif
