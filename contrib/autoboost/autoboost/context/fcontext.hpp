
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_CONTEXT_FCONTEXT_H
#define AUTOBOOST_CONTEXT_FCONTEXT_H

#if defined(__PGI)
#include <stdint.h>
#endif

#if defined(_WIN32_WCE)
typedef int intptr_t;
#endif

#include <autoboost/config.hpp>
#include <autoboost/cstdint.hpp>

#include <autoboost/context/detail/config.hpp>

#ifdef AUTOBOOST_HAS_ABI_HEADERS
# include AUTOBOOST_ABI_PREFIX
#endif

namespace autoboost {
namespace context {

typedef void*   fcontext_t;

extern "C" AUTOBOOST_CONTEXT_DECL
intptr_t AUTOBOOST_CONTEXT_CALLDECL jump_fcontext( fcontext_t * ofc, fcontext_t nfc,
                                               intptr_t vp, bool preserve_fpu = false);
extern "C" AUTOBOOST_CONTEXT_DECL
fcontext_t AUTOBOOST_CONTEXT_CALLDECL make_fcontext( void * sp, std::size_t size, void (* fn)( intptr_t) );

}}

#ifdef AUTOBOOST_HAS_ABI_HEADERS
# include AUTOBOOST_ABI_SUFFIX
#endif

#endif // AUTOBOOST_CONTEXT_FCONTEXT_H

