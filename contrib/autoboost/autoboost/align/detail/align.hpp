/*
Copyright 2014-2016 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AUTOBOOST_ALIGN_DETAIL_ALIGN_HPP
#define AUTOBOOST_ALIGN_DETAIL_ALIGN_HPP

#include <autoboost/align/detail/is_alignment.hpp>
#include <autoboost/assert.hpp>

namespace autoboost {
namespace alignment {

inline void*
align(std::size_t alignment, std::size_t size, void*& ptr,
    std::size_t& space)
{
    AUTOBOOST_ASSERT(detail::is_alignment(alignment));
    if (size <= space) {
        char* p = reinterpret_cast<char*>((reinterpret_cast<std::
            size_t>(ptr) + alignment - 1) & ~(alignment - 1));
        std::size_t n = space - (p - static_cast<char*>(ptr));
        if (size <= n) {
            ptr = p;
            space = n;
            return p;
        }
    }
    return 0;
}

} /* alignment */
} /* autoboost */

#endif
