
//          Copyright Oliver Kowalke 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_CONTEXT_DETAIL_CONFIG_H
#define AUTOBOOST_CONTEXT_DETAIL_CONFIG_H

// required for SD-6 compile-time integer sequences
#include <utility>

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

#if defined(AUTOBOOST_USE_SEGMENTED_STACKS)
# if ! ( (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6) ) ) || \
         (defined(__clang__) && (__clang_major__ > 2 || ( __clang_major__ == 2 && __clang_minor__ > 3) ) ) )
#  error "compiler does not support segmented_stack stacks"
# endif
# define AUTOBOOST_CONTEXT_SEGMENTS 10
#endif


#define AUTOBOOST_CONTEXT_NO_CXX14_INTEGER_SEQUENCE
// use rd6 macros for std::integer_sequence
#if defined(__cpp_lib_integer_sequence) && __cpp_lib_integer_sequence >= 201304
# undef AUTOBOOST_CONTEXT_NO_CXX14_INTEGER_SEQUENCE
#endif
// workaroud: MSVC 14 does not provide macros to test for compile-time integer sequence
#if _MSC_VER > 1800 // _MSC_VER == 1800 -> MS Visual Studio 2013
# undef AUTOBOOST_CONTEXT_NO_INDEX_SEQUENCE
#endif
// workaround: Xcode clang feature detection
#if ! defined(__cpp_lib_integer_sequence) && __cpp_lib_integer_sequence >= 201304
# if _LIBCPP_STD_VER > 11
#  undef AUTOBOOST_CONTEXT_NO_CXX14_INTEGER_SEQUENCE
# endif
#endif

// workaroud: MSVC 14 does support constexpr
#if _MSC_VER > 1800 // _MSC_VER == 1800 -> MS Visual Studio 2013
# undef AUTOBOOST_NO_CXX11_CONSTEXPR
#endif

#undef AUTOBOOST_CONTEXT_NO_CXX11
#if defined(AUTOBOOST_NO_CXX11_AUTO_DECLARATIONS) || \
    defined(AUTOBOOST_NO_CXX11_CONSTEXPR) || \
    defined(AUTOBOOST_NO_CXX11_DEFAULTED_FUNCTIONS) || \
    defined(AUTOBOOST_NO_CXX11_FINAL) || \
    defined(AUTOBOOST_NO_CXX11_HDR_TUPLE) || \
    defined(AUTOBOOST_NO_CXX11_NOEXCEPT) || \
    defined(AUTOBOOST_NO_CXX11_NULLPTR) || \
    defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES) || \
    defined(AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES) || \
    defined(AUTOBOOST_NO_CXX11_UNIFIED_INITIALISATION_SYNTAX) || \
    defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES) || \
    defined(AUTOBOOST_NO_HDR_ATOMIC) || \
    defined(AUTOBOOST_NO_HDR_TUPLE)
# define AUTOBOOST_CONTEXT_NO_CXX11
#endif

#if ! defined(AUTOBOOST_EXECUTION_CONTEXT)
# if defined(AUTOBOOST_USE_SEGMENTED_STACKS)
#  define AUTOBOOST_EXECUTION_CONTEXT 1
# else
#  define AUTOBOOST_EXECUTION_CONTEXT 2
# endif
#endif

#endif // AUTOBOOST_CONTEXT_DETAIL_CONFIG_H
