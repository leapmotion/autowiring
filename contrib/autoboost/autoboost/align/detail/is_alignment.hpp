/*
 (c) 2014 Glen Joseph Fernandes
 glenjofe at gmail dot com

 Distributed under the Boost Software
 License, Version 1.0.
 http://autoboost.org/LICENSE_1_0.txt
*/
#ifndef AUTOBOOST_ALIGN_DETAIL_IS_ALIGNMENT_HPP
#define AUTOBOOST_ALIGN_DETAIL_IS_ALIGNMENT_HPP

#include <autoboost/config.hpp>
#include <cstddef>

namespace autoboost {
    namespace alignment {
        namespace detail {
            AUTOBOOST_CONSTEXPR inline bool is_alignment(std::size_t
                value) AUTOBOOST_NOEXCEPT
            {
                return (value > 0) && ((value & (value - 1)) == 0);
            }
        }
    }
}

#endif
