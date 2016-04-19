
//          Copyright Oliver Kowalke 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_CONTEXT_FIXEDSIZE_H
#define AUTOBOOST_CONTEXT_FIXEDSIZE_H

#include <cstddef>
#include <cstdlib>
#include <new>

#include <autoboost/assert.hpp>
#include <autoboost/config.hpp>

#include <autoboost/context/detail/config.hpp>
#include <autoboost/context/stack_context.hpp>
#include <autoboost/context/stack_traits.hpp>

#if defined(AUTOBOOST_USE_VALGRIND)
#include <valgrind/valgrind.h>
#endif

#ifdef AUTOBOOST_HAS_ABI_HEADERS
#  include AUTOBOOST_ABI_PREFIX
#endif

namespace autoboost {
namespace context {

template< typename traitsT >
class basic_fixedsize_stack {
private:
#if defined(AUTOBOOST_USE_WINFIBERS)
    friend class execution_context;
#endif

    std::size_t     size_;

public:
    typedef traitsT traits_type;

    basic_fixedsize_stack( std::size_t size = traits_type::default_size() ) :
        size_( size) {
        AUTOBOOST_ASSERT( traits_type::minimum_size() <= size_);
        AUTOBOOST_ASSERT( traits_type::is_unbounded() || ( traits_type::maximum_size() >= size_) );
    }

    stack_context allocate() {
        void * vp = std::malloc( size_);
        if ( ! vp) throw std::bad_alloc();

        stack_context sctx;
        sctx.size = size_;
        sctx.sp = static_cast< char * >( vp) + sctx.size;
#if defined(AUTOBOOST_USE_VALGRIND)
        sctx.valgrind_stack_id = VALGRIND_STACK_REGISTER( sctx.sp, vp);
#endif
        return sctx;
    }

    void deallocate( stack_context & sctx) {
        AUTOBOOST_ASSERT( sctx.sp);
#if ! defined(AUTOBOOST_USE_WINFIBERS)
        AUTOBOOST_ASSERT( traits_type::minimum_size() <= sctx.size);
        AUTOBOOST_ASSERT( traits_type::is_unbounded() || ( traits_type::maximum_size() >= sctx.size) );
#endif

#if defined(AUTOBOOST_USE_VALGRIND)
        VALGRIND_STACK_DEREGISTER( sctx.valgrind_stack_id);
#endif

        void * vp = static_cast< char * >( sctx.sp) - sctx.size;
        std::free( vp);
    }
};

typedef basic_fixedsize_stack< stack_traits >  fixedsize_stack;
# if ! defined(AUTOBOOST_USE_SEGMENTED_STACKS)
typedef fixedsize_stack default_stack;
# endif

}}

#ifdef AUTOBOOST_HAS_ABI_HEADERS
#  include AUTOBOOST_ABI_SUFFIX
#endif

#endif // AUTOBOOST_CONTEXT_FIXEDSIZE_H
