/*
Copyright 2014 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AUTOBOOST_ALIGN_DETAIL_IS_ALIGNED_HPP
#define AUTOBOOST_ALIGN_DETAIL_IS_ALIGNED_HPP

#include <autoboost/align/detail/is_alignment.hpp>
#include <autoboost/assert.hpp>

namespace autoboost {
namespace alignment {

inline bool
is_aligned(const void* ptr, std::size_t alignment) AUTOBOOST_NOEXCEPT
{
    AUTOBOOST_ASSERT(detail::is_alignment(alignment));
    return (reinterpret_cast<std::size_t>(ptr) & (alignment - 1)) == 0;
}

inline bool
is_aligned(std::size_t alignment, const void* ptr) AUTOBOOST_NOEXCEPT
{
    AUTOBOOST_ASSERT(detail::is_alignment(alignment));
    return (reinterpret_cast<std::size_t>(ptr) & (alignment - 1)) == 0;
}

} /* alignment */
} /* autoboost */

#endif
