/*
Copyright 2014 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AUTOBOOST_ALIGN_DETAIL_ALIGNMENT_OF_CODEGEAR_HPP
#define AUTOBOOST_ALIGN_DETAIL_ALIGNMENT_OF_CODEGEAR_HPP

#include <autoboost/align/detail/integral_constant.hpp>
#include <cstddef>

namespace autoboost {
namespace alignment {
namespace detail {

template<class T>
struct alignment_of
    : integral_constant<std::size_t, alignof(T)> { };

} /* detail */
} /* alignment */
} /* autoboost */

#endif
