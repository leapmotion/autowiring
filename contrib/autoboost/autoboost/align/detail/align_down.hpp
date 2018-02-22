/*
Copyright 2015 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AUTOBOOST_ALIGN_DETAIL_ALIGN_DOWN_HPP
#define AUTOBOOST_ALIGN_DETAIL_ALIGN_DOWN_HPP

#include <autoboost/align/detail/is_alignment.hpp>
#include <autoboost/assert.hpp>

namespace autoboost {
namespace alignment {

inline void*
align_down(void* ptr, std::size_t alignment) AUTOBOOST_NOEXCEPT
{
    AUTOBOOST_ASSERT(detail::is_alignment(alignment));
    return reinterpret_cast<void*>(reinterpret_cast<std::
        size_t>(ptr) & ~(alignment - 1));
}

} /* alignment */
} /* autoboost */

#endif
