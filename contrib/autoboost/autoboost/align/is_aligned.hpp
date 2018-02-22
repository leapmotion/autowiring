/*
Copyright 2014 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)

*/
#ifndef AUTOBOOST_ALIGN_IS_ALIGNED_HPP
#define AUTOBOOST_ALIGN_IS_ALIGNED_HPP

#include <autoboost/align/detail/is_aligned.hpp>

namespace autoboost {
namespace alignment {

AUTOBOOST_CONSTEXPR inline bool
is_aligned(std::size_t value, std::size_t alignment) AUTOBOOST_NOEXCEPT
{
    return (value & (alignment - 1)) == 0;
}

} /* alignment */
} /* autoboost */

#endif
