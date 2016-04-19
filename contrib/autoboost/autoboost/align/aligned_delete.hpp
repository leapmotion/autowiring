/*
(c) 2014 Glen Joseph Fernandes
<glenjofe -at- gmail.com>

Distributed under the Boost Software
License, Version 1.0.
http://autoboost.org/LICENSE_1_0.txt
*/
#ifndef AUTOBOOST_ALIGN_ALIGNED_DELETE_HPP
#define AUTOBOOST_ALIGN_ALIGNED_DELETE_HPP

#include <autoboost/config.hpp>
#include <autoboost/align/aligned_alloc.hpp>
#include <autoboost/align/aligned_delete_forward.hpp>

namespace autoboost {
namespace alignment {

class aligned_delete {
public:
    template<class T>
    void operator()(T* ptr) const
        AUTOBOOST_NOEXCEPT_IF(AUTOBOOST_NOEXCEPT_EXPR(ptr->~T())) {
        if (ptr) {
            ptr->~T();
            ::autoboost::alignment::aligned_free(ptr);
        }
    }
};

} /* .alignment */
} /* .boost */

#endif
