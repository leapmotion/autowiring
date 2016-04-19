/*
(c) 2014 Glen Joseph Fernandes
<glenjofe -at- gmail.com>

Distributed under the Boost Software
License, Version 1.0.
http://autoboost.org/LICENSE_1_0.txt
*/
#ifndef AUTOBOOST_ALIGN_DETAIL_IS_ALIGNED_HPP
#define AUTOBOOST_ALIGN_DETAIL_IS_ALIGNED_HPP

#include <autoboost/assert.hpp>
#include <autoboost/config.hpp>
#include <autoboost/align/detail/address.hpp>
#include <autoboost/align/detail/is_alignment.hpp>
#include <cstddef>

namespace autoboost {
namespace alignment {

inline bool is_aligned(std::size_t alignment, const void* ptr)
    AUTOBOOST_NOEXCEPT
{
    AUTOBOOST_ASSERT(detail::is_alignment(alignment));
    return (detail::address(ptr) & (alignment - 1)) == 0;
}

} /* .alignment */
} /* .boost */

#endif
