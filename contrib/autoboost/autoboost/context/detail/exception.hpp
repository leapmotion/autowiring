
//          Copyright Oliver Kowalke 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_CONTEXT_DETAIL_EXCEPTION_H
#define AUTOBOOST_CONTEXT_DETAIL_EXCEPTION_H

#include <autoboost/config.hpp>

#include <autoboost/context/detail/fcontext.hpp>

#ifdef AUTOBOOST_HAS_ABI_HEADERS
# include AUTOBOOST_ABI_PREFIX
#endif

namespace autoboost {
namespace context {
namespace detail {

struct forced_unwind {
    fcontext_t  fctx;

    forced_unwind( fcontext_t fctx_) :
        fctx( fctx_) {
    }
};

}}}

#ifdef AUTOBOOST_HAS_ABI_HEADERS
#include AUTOBOOST_ABI_SUFFIX
#endif

#endif // AUTOBOOST_CONTEXT_DETAIL_EXCEPTION_H
