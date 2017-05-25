
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_CONTEXT_DETAIL_FCONTEXT_I386H
#define AUTOBOOST_CONTEXT_DETAIL_FCONTEXT_I386H

#include <cstddef>

#include <autoboost/config.hpp>
#include <autoboost/cstdint.hpp>

#include <autoboost/context/detail/config.hpp>

#ifdef AUTOBOOST_HAS_ABI_HEADERS
# include AUTOBOOST_ABI_PREFIX
#endif

namespace autoboost {
namespace context {

extern "C" {

#define AUTOBOOST_CONTEXT_CALLDECL __attribute__((cdecl))

struct stack_t
{
    void            *   sp;
    std::size_t         size;

    stack_t() :
        sp( 0), size( 0)
    {}
};

struct fcontext_t
{
    autoboost::uint32_t     fc_greg[6];
    stack_t             fc_stack;
    autoboost::uint32_t     fc_freg[2];

    fcontext_t() :
        fc_greg(),
        fc_stack(),
        fc_freg()
    {}
};

}

}}

#ifdef AUTOBOOST_HAS_ABI_HEADERS
# include AUTOBOOST_ABI_SUFFIX
#endif

#endif // AUTOBOOST_CONTEXT_DETAIL_FCONTEXT_I386_H
