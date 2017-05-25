
//          Copyright Oliver Kowalke 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_CONTEXT_STACK_CONTEXT_H
#define AUTOBOOST_CONTEXT_STACK_CONTEXT_H

#include <cstddef>

#include <autoboost/config.hpp>

#include <autoboost/context/detail/config.hpp>

#ifdef AUTOBOOST_HAS_ABI_HEADERS
#  include AUTOBOOST_ABI_PREFIX
#endif

namespace autoboost {
namespace context {

#if ! defined(AUTOBOOST_CONTEXT_NO_CXX11)
struct stack_context {
# if defined(AUTOBOOST_USE_SEGMENTED_STACKS)
    typedef void *  segments_context[AUTOBOOST_CONTEXT_SEGMENTS];
# endif

    std::size_t             size{ 0 };
    void                *   sp{ nullptr };
# if defined(AUTOBOOST_USE_SEGMENTED_STACKS)
    segments_context        segments_ctx{};
# endif
# if defined(AUTOBOOST_USE_VALGRIND)
    unsigned                valgrind_stack_id{ 0 };
# endif
};
#else
struct stack_context {
# if defined(AUTOBOOST_USE_SEGMENTED_STACKS)
    typedef void *  segments_context[AUTOBOOST_CONTEXT_SEGMENTS];
# endif

    std::size_t             size;
    void                *   sp;
# if defined(AUTOBOOST_USE_SEGMENTED_STACKS)
    segments_context        segments_ctx;
# endif
# if defined(AUTOBOOST_USE_VALGRIND)
    unsigned                valgrind_stack_id;
# endif

    stack_context() :
        size( 0),
        sp( 0)
# if defined(AUTOBOOST_USE_SEGMENTED_STACKS)
        , segments_ctx()
# endif
# if defined(AUTOBOOST_USE_VALGRIND)
        , valgrind_stack_id( 0)
# endif
        {}
};
#endif

}}

#ifdef AUTOBOOST_HAS_ABI_HEADERS
#  include AUTOBOOST_ABI_SUFFIX
#endif

#endif // AUTOBOOST_CONTEXT_STACK_CONTEXT_H
