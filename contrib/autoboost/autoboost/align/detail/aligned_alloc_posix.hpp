/*
Copyright 2014 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AUTOBOOST_ALIGN_DETAIL_ALIGNED_ALLOC_POSIX_HPP
#define AUTOBOOST_ALIGN_DETAIL_ALIGNED_ALLOC_POSIX_HPP

#include <autoboost/align/detail/is_alignment.hpp>
#include <autoboost/assert.hpp>
#include <stdlib.h>

namespace autoboost {
namespace alignment {

inline void*
aligned_alloc(std::size_t alignment, std::size_t size) AUTOBOOST_NOEXCEPT
{
    AUTOBOOST_ASSERT(detail::is_alignment(alignment));
    if (alignment < sizeof(void*)) {
        alignment = sizeof(void*);
    }
    void* p;
    if (::posix_memalign(&p, alignment, size) != 0) {
        p = 0;
    }
    return p;
}

inline void
aligned_free(void* ptr) AUTOBOOST_NOEXCEPT
{
    ::free(ptr);
}

} /* alignment */
} /* autoboost */

#endif
