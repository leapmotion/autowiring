
//          Copyright Oliver Kowalke 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_CONTEXT_PREALLOCATED_H
#define AUTOBOOST_CONTEXT_PREALLOCATED_H

#include <cstddef>

#include <autoboost/config.hpp>

#include <autoboost/context/detail/config.hpp>

#ifdef AUTOBOOST_HAS_ABI_HEADERS
# include AUTOBOOST_ABI_PREFIX
#endif

namespace autoboost {
namespace context {

struct preallocated {
    void        *   sp;
    std::size_t     size;
    stack_context   sctx;

    preallocated( void * sp_, std::size_t size_, stack_context sctx_) noexcept :
        sp( sp_), size( size_), sctx( sctx_) {
    }
};

}}

#ifdef AUTOBOOST_HAS_ABI_HEADERS
# include AUTOBOOST_ABI_SUFFIX
#endif

#endif // AUTOBOOST_CONTEXT_PREALLOCATED_H
