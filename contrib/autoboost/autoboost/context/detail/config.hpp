
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_CONTEXT_DETAIL_CONFIG_H
#define AUTOBOOST_CONTEXT_DETAIL_CONFIG_H

#include <autoboost/config.hpp>
#include <autoboost/detail/workaround.hpp>

#ifdef AUTOBOOST_CONTEXT_DECL
# undef AUTOBOOST_CONTEXT_DECL
#endif

#if (defined(AUTOBOOST_ALL_DYN_LINK) || defined(AUTOBOOST_CONTEXT_DYN_LINK) ) && ! defined(AUTOBOOST_CONTEXT_STATIC_LINK)
# if defined(AUTOBOOST_CONTEXT_SOURCE)
#  define AUTOBOOST_CONTEXT_DECL AUTOBOOST_SYMBOL_EXPORT
#  define AUTOBOOST_CONTEXT_BUILD_DLL
# else
#  define AUTOBOOST_CONTEXT_DECL AUTOBOOST_SYMBOL_IMPORT
# endif
#endif

#if ! defined(AUTOBOOST_CONTEXT_DECL)
# define AUTOBOOST_CONTEXT_DECL
#endif

#if ! defined(AUTOBOOST_CONTEXT_SOURCE) && ! defined(AUTOBOOST_ALL_NO_LIB) && ! defined(AUTOBOOST_CONTEXT_NO_LIB)
# define AUTOBOOST_LIB_NAME autoboost_context
# if defined(AUTOBOOST_ALL_DYN_LINK) || defined(AUTOBOOST_CONTEXT_DYN_LINK)
#  define AUTOBOOST_DYN_LINK
# endif
# include <autoboost/config/auto_link.hpp>
#endif

#undef AUTOBOOST_CONTEXT_CALLDECL
#if (defined(i386) || defined(__i386__) || defined(__i386) \
    || defined(__i486__) || defined(__i586__) || defined(__i686__) \
    || defined(__X86__) || defined(_X86_) || defined(__THW_INTEL__) \
    || defined(__I86__) || defined(__INTEL__) || defined(__IA32__) \
    || defined(_M_IX86) || defined(_I86_)) && defined(AUTOBOOST_WINDOWS)
# define AUTOBOOST_CONTEXT_CALLDECL __cdecl
#else
# define AUTOBOOST_CONTEXT_CALLDECL
#endif

#endif // AUTOBOOST_CONTEXT_DETAIL_CONFIG_H
