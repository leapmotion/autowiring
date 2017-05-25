/*
Copyright 2014-2015 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AUTOBOOST_ALIGN_DETAIL_ALIGNED_ALLOC_HPP
#define AUTOBOOST_ALIGN_DETAIL_ALIGNED_ALLOC_HPP

#include <autoboost/align/detail/is_alignment.hpp>
#include <autoboost/align/align.hpp>
#include <autoboost/align/alignment_of.hpp>
#include <autoboost/assert.hpp>
#include <cstdlib>

namespace autoboost {
namespace alignment {

inline void*
aligned_alloc(std::size_t alignment, std::size_t size) AUTOBOOST_NOEXCEPT
{
    AUTOBOOST_ASSERT(detail::is_alignment(alignment));
    enum {
        N = alignment_of<void*>::value
    };
    if (alignment < N) {
        alignment = N;
    }
    std::size_t n = size + alignment - N;
    void* p = std::malloc(sizeof(void*) + n);
    if (p) {
        void* r = static_cast<char*>(p) + sizeof(void*);
        (void)align(alignment, size, r, n);
        *(static_cast<void**>(r) - 1) = p;
        p = r;
    }
    return p;
}

inline void
aligned_free(void* ptr) AUTOBOOST_NOEXCEPT
{
    if (ptr) {
        std::free(*(static_cast<void**>(ptr) - 1));
    }
}

} /* alignment */
} /* autoboost */

#endif
