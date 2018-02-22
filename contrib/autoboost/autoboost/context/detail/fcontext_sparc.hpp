//          Copyright Martin Husemann 2012
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_CTX_DETAIL_FCONTEXT_SPARC_H
#define AUTOBOOST_CTX_DETAIL_FCONTEXT_SPARC_H

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

#define AUTOBOOST_CONTEXT_CALLDECL

// if defined(_LP64) we are compiling for sparc64, otherwise it is 32 bit
// sparc.


struct stack_t
{
    void    *   sp;
    std::size_t size;

    stack_t() :
        sp( 0), size( 0)
    {}
};

struct fp_t
{
#ifdef _LP64
    autoboost::uint64_t     fp_freg[32];
    autoboost::uint64_t	fp_fprs, fp_fsr;
#else
    autoboost::uint64_t     fp_freg[16];
    autoboost::uint32_t	fp_fsr;
#endif

    fp_t() :
        fp_freg(),
#ifdef _LP64
	fp_fprs(),
#endif
	fp_fsr()
    {}
}
#ifdef _LP64
		 __attribute__((__aligned__(64)))	// allow VIS instructions to be used
#endif
;

struct fcontext_t
{
    fp_t                fc_fp;	// fpu stuff first, for easier alignement
#ifdef _LP64
    autoboost::uint64_t
#else
    autoboost::uint32_t
#endif
			fc_greg[8];
    stack_t             fc_stack;

    fcontext_t() :
        fc_fp(),
        fc_greg(),
        fc_stack()
    {}
};

}

}}

#ifdef AUTOBOOST_HAS_ABI_HEADERS
# include AUTOBOOST_ABI_SUFFIX
#endif

#endif // AUTOBOOST_CTX_DETAIL_FCONTEXT_SPARC_H
