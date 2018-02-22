/*
Copyright 2014-2015 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AUTOBOOST_ALIGN_ALIGNED_DELETE_HPP
#define AUTOBOOST_ALIGN_ALIGNED_DELETE_HPP

#include <autoboost/align/aligned_alloc.hpp>
#include <autoboost/align/aligned_delete_forward.hpp>

namespace autoboost {
namespace alignment {

struct aligned_delete {
    template<class T>
    void operator()(T* ptr) const
        AUTOBOOST_NOEXCEPT_IF(AUTOBOOST_NOEXCEPT_EXPR(ptr->~T())) {
        if (ptr) {
            ptr->~T();
            autoboost::alignment::aligned_free(ptr);
        }
    }
};

} /* alignment */
} /* autoboost */

#endif
