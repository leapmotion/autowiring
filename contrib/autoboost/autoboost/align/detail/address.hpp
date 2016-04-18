/*
 (c) 2014 Glen Joseph Fernandes
 glenjofe at gmail dot com

 Distributed under the Boost Software
 License, Version 1.0.
 http://autoboost.org/LICENSE_1_0.txt
*/
#ifndef AUTOBOOST_ALIGN_DETAIL_ADDRESS_HPP
#define AUTOBOOST_ALIGN_DETAIL_ADDRESS_HPP

#include <autoboost/cstdint.hpp>
#include <cstddef>

namespace autoboost {
    namespace alignment {
        namespace detail {
#if defined(AUTOBOOST_HAS_INTPTR_T)
            typedef autoboost::uintptr_t address_t;
#else
            typedef std::size_t address_t;
#endif
        }
    }
}

#endif
