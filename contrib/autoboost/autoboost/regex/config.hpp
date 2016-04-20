/*
 *
 * Copyright (c) 1998-2002
 * John Maddock
 *
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

 /*
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE         config.hpp
  *   VERSION      see <autoboost/version.hpp>
  *   DESCRIPTION: regex extended config setup.
  */

#ifndef AUTOBOOST_REGEX_CONFIG_HPP
#define AUTOBOOST_REGEX_CONFIG_HPP
/*
 * Borland C++ Fix/error check
 * this has to go *before* we include any std lib headers:
 */
#if defined(__BORLANDC__)
#  include <autoboost/regex/config/borland.hpp>
#endif

/*****************************************************************************
 *
 *  Include all the headers we need here:
 *
 ****************************************************************************/

#ifdef __cplusplus

#  ifndef AUTOBOOST_REGEX_USER_CONFIG
#     define AUTOBOOST_REGEX_USER_CONFIG <autoboost/regex/user.hpp>
#  endif

#  include AUTOBOOST_REGEX_USER_CONFIG

#  include <autoboost/config.hpp>

#else
   /*
    * C build,
    * don't include <autoboost/config.hpp> because that may
    * do C++ specific things in future...
    */
#  include <stdlib.h>
#  include <stddef.h>
#  ifdef _MSC_VER
#     define AUTOBOOST_MSVC _MSC_VER
#  endif
#endif

/*****************************************************************************
 *
 *  Boilerplate regex config options:
 *
 ****************************************************************************/

/* Obsolete macro, use AUTOBOOST_VERSION instead: */
#define AUTOBOOST_RE_VERSION 320

/* fix: */
#if defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

/*
 * Fix for gcc prior to 3.4: std::ctype<wchar_t> doesn't allow
 * masks to be combined, for example:
 * std::use_facet<std::ctype<wchar_t> >.is(std::ctype_base::lower|std::ctype_base::upper, L'a');
 * returns *false*.
 */
#ifdef __GLIBCPP__
#  define AUTOBOOST_REGEX_BUGGY_CTYPE_FACET
#endif

/*
 * Intel C++ before 8.0 ends up with unresolved externals unless we turn off
 * extern template support:
 */
#if defined(AUTOBOOST_INTEL) && defined(__cplusplus) && (AUTOBOOST_INTEL <= 800)
#  define AUTOBOOST_REGEX_NO_EXTERNAL_TEMPLATES
#endif
/*
 * Visual C++ doesn't support external templates with C++ extensions turned off:
 */
#if defined(_MSC_VER) && !defined(_MSC_EXTENSIONS)
#  define AUTOBOOST_REGEX_NO_EXTERNAL_TEMPLATES
#endif
/*
 * Shared regex lib will crash without this, frankly it looks a lot like a gcc bug:
 */
#if defined(__MINGW32__)
#  define AUTOBOOST_REGEX_NO_EXTERNAL_TEMPLATES
#endif

/*
 * If there isn't good enough wide character support then there will
 * be no wide character regular expressions:
 */
#if (defined(AUTOBOOST_NO_CWCHAR) || defined(AUTOBOOST_NO_CWCTYPE) || defined(AUTOBOOST_NO_STD_WSTRING))
#  if !defined(AUTOBOOST_NO_WREGEX)
#     define AUTOBOOST_NO_WREGEX
#  endif
#else
#  if defined(__sgi) && (defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION))
      /* STLPort on IRIX is misconfigured: <cwctype> does not compile
       * as a temporary fix include <wctype.h> instead and prevent inclusion
       * of STLPort version of <cwctype> */
#     include <wctype.h>
#     define __STLPORT_CWCTYPE
#     define _STLP_CWCTYPE
#  endif

#ifdef __cplusplus
#  include <autoboost/regex/config/cwchar.hpp>
#endif

#endif

/*
 * If Win32 support has been disabled for autoboost in general, then
 * it is for regex in particular:
 */
#if defined(AUTOBOOST_DISABLE_WIN32) && !defined(AUTOBOOST_REGEX_NO_W32)
#  define AUTOBOOST_REGEX_NO_W32
#endif

/* disable our own file-iterators and mapfiles if we can't
 * support them: */
#if !defined(AUTOBOOST_HAS_DIRENT_H) && !(defined(_WIN32) && !defined(AUTOBOOST_REGEX_NO_W32))
#  define AUTOBOOST_REGEX_NO_FILEITER
#endif

/* backwards compatibitity: */
#if defined(AUTOBOOST_RE_NO_LIB)
#  define AUTOBOOST_REGEX_NO_LIB
#endif

#if defined(__GNUC__) && (defined(_WIN32) || defined(__CYGWIN__))
/* gcc on win32 has problems if you include <windows.h>
   (sporadically generates bad code). */
#  define AUTOBOOST_REGEX_NO_W32
#endif
#if defined(__COMO__) && !defined(AUTOBOOST_REGEX_NO_W32) && !defined(_MSC_EXTENSIONS)
#  define AUTOBOOST_REGEX_NO_W32
#endif

/*****************************************************************************
 *
 *  Wide character workarounds:
 *
 ****************************************************************************/

/*
 * define AUTOBOOST_REGEX_HAS_OTHER_WCHAR_T when wchar_t is a native type, but the users
 * code may be built with wchar_t as unsigned short: basically when we're building
 * with MSVC and the /Zc:wchar_t option we place some extra unsigned short versions
 * of the non-inline functions in the library, so that users can still link to the lib,
 * irrespective of whether their own code is built with /Zc:wchar_t.
 * Note that this does NOT WORK with VC10 when the C++ locale is in effect as
 * the locale's <unsigned short> facets simply do not compile in that case.
 */
#if defined(__cplusplus) && (defined(AUTOBOOST_MSVC) || defined(__ICL)) && !defined(AUTOBOOST_NO_INTRINSIC_WCHAR_T) && defined(AUTOBOOST_WINDOWS) && !defined(__SGI_STL_PORT) && !defined(_STLPORT_VERSION) && !defined(AUTOBOOST_RWSTD_VER) && ((_MSC_VER < 1600) || !defined(AUTOBOOST_REGEX_USE_CPP_LOCALE))
#  define AUTOBOOST_REGEX_HAS_OTHER_WCHAR_T
#  ifdef AUTOBOOST_MSVC
#     pragma warning(push)
#     pragma warning(disable : 4251 4231)
#     if AUTOBOOST_MSVC < 1600
#        pragma warning(disable : 4660)
#     endif
#  endif
#  if defined(_DLL) && defined(AUTOBOOST_MSVC) && (AUTOBOOST_MSVC < 1600)
#     include <string>
      extern template class __declspec(dllimport) std::basic_string<unsigned short>;
#  endif
#  ifdef AUTOBOOST_MSVC
#     pragma warning(pop)
#  endif
#endif


/*****************************************************************************
 *
 *  Set up dll import/export options:
 *
 ****************************************************************************/

#ifndef AUTOBOOST_SYMBOL_EXPORT
#  define AUTOBOOST_SYMBOL_EXPORT
#  define AUTOBOOST_SYMBOL_IMPORT
#endif

#if (defined(AUTOBOOST_REGEX_DYN_LINK) || defined(AUTOBOOST_ALL_DYN_LINK)) && !defined(AUTOBOOST_REGEX_STATIC_LINK)
#  if defined(AUTOBOOST_REGEX_SOURCE)
#     define AUTOBOOST_REGEX_DECL AUTOBOOST_SYMBOL_EXPORT
#     define AUTOBOOST_REGEX_BUILD_DLL
#  else
#     define AUTOBOOST_REGEX_DECL AUTOBOOST_SYMBOL_IMPORT
#  endif
#else
#  define AUTOBOOST_REGEX_DECL
#endif

/*****************************************************************************
 *
 *  Set up function call type:
 *
 ****************************************************************************/

#if defined(AUTOBOOST_MSVC) && defined(_MSC_EXTENSIONS)
#if defined(_DEBUG) || defined(__MSVC_RUNTIME_CHECKS) || defined(_MANAGED) || defined(AUTOBOOST_REGEX_NO_FASTCALL)
#  define AUTOBOOST_REGEX_CALL __cdecl
#else
#  define AUTOBOOST_REGEX_CALL __fastcall
#endif
#  define AUTOBOOST_REGEX_CCALL __cdecl
#endif

#if defined(__BORLANDC__) && !defined(AUTOBOOST_DISABLE_WIN32)
#  define AUTOBOOST_REGEX_CALL __fastcall
#  define AUTOBOOST_REGEX_CCALL __stdcall
#endif

#ifndef AUTOBOOST_REGEX_CALL
#  define AUTOBOOST_REGEX_CALL
#endif
#ifndef AUTOBOOST_REGEX_CCALL
#define AUTOBOOST_REGEX_CCALL
#endif

/*****************************************************************************
 *
 *  Set up localisation model:
 *
 ****************************************************************************/

/* backwards compatibility: */
#ifdef AUTOBOOST_RE_LOCALE_C
#  define AUTOBOOST_REGEX_USE_C_LOCALE
#endif

#ifdef AUTOBOOST_RE_LOCALE_CPP
#  define AUTOBOOST_REGEX_USE_CPP_LOCALE
#endif

#if defined(__CYGWIN__)
#  define AUTOBOOST_REGEX_USE_C_LOCALE
#endif

/* Win32 defaults to native Win32 locale: */
#if defined(_WIN32) && !defined(AUTOBOOST_REGEX_USE_WIN32_LOCALE) && !defined(AUTOBOOST_REGEX_USE_C_LOCALE) && !defined(AUTOBOOST_REGEX_USE_CPP_LOCALE) && !defined(AUTOBOOST_REGEX_NO_W32)
#  define AUTOBOOST_REGEX_USE_WIN32_LOCALE
#endif
/* otherwise use C++ locale if supported: */
#if !defined(AUTOBOOST_REGEX_USE_WIN32_LOCALE) && !defined(AUTOBOOST_REGEX_USE_C_LOCALE) && !defined(AUTOBOOST_REGEX_USE_CPP_LOCALE) && !defined(AUTOBOOST_NO_STD_LOCALE)
#  define AUTOBOOST_REGEX_USE_CPP_LOCALE
#endif
/* otherwise use C+ locale: */
#if !defined(AUTOBOOST_REGEX_USE_WIN32_LOCALE) && !defined(AUTOBOOST_REGEX_USE_C_LOCALE) && !defined(AUTOBOOST_REGEX_USE_CPP_LOCALE)
#  define AUTOBOOST_REGEX_USE_C_LOCALE
#endif

#ifndef AUTOBOOST_REGEX_MAX_STATE_COUNT
#  define AUTOBOOST_REGEX_MAX_STATE_COUNT 100000000
#endif


/*****************************************************************************
 *
 *  Error Handling for exception free compilers:
 *
 ****************************************************************************/

#ifdef AUTOBOOST_NO_EXCEPTIONS
/*
 * If there are no exceptions then we must report critical-errors
 * the only way we know how; by terminating.
 */
#include <stdexcept>
#include <string>
#include <autoboost/throw_exception.hpp>

#  define AUTOBOOST_REGEX_NOEH_ASSERT(x)\
if(0 == (x))\
{\
   std::string s("Error: critical regex++ failure in: ");\
   s.append(#x);\
   std::runtime_error e(s);\
   autoboost::throw_exception(e);\
}
#else
/*
 * With exceptions then error handling is taken care of and
 * there is no need for these checks:
 */
#  define AUTOBOOST_REGEX_NOEH_ASSERT(x)
#endif


/*****************************************************************************
 *
 *  Stack protection under MS Windows:
 *
 ****************************************************************************/

#if !defined(AUTOBOOST_REGEX_NO_W32) && !defined(AUTOBOOST_REGEX_V3)
#  if(defined(_WIN32) || defined(_WIN64) || defined(_WINCE)) \
        && !defined(__GNUC__) \
        && !(defined(__BORLANDC__) && (__BORLANDC__ >= 0x600)) \
        && !(defined(__MWERKS__) && (__MWERKS__ <= 0x3003))
#     define AUTOBOOST_REGEX_HAS_MS_STACK_GUARD
#  endif
#elif defined(AUTOBOOST_REGEX_HAS_MS_STACK_GUARD)
#  undef AUTOBOOST_REGEX_HAS_MS_STACK_GUARD
#endif

#if defined(__cplusplus) && defined(AUTOBOOST_REGEX_HAS_MS_STACK_GUARD)

namespace autoboost{
namespace re_detail{

AUTOBOOST_REGEX_DECL void AUTOBOOST_REGEX_CALL reset_stack_guard_page();

}
}

#endif


/*****************************************************************************
 *
 *  Algorithm selection and configuration:
 *
 ****************************************************************************/

#if !defined(AUTOBOOST_REGEX_RECURSIVE) && !defined(AUTOBOOST_REGEX_NON_RECURSIVE)
#  if defined(AUTOBOOST_REGEX_HAS_MS_STACK_GUARD) && !defined(_STLP_DEBUG) && !defined(__STL_DEBUG) && !(defined(AUTOBOOST_MSVC) && (AUTOBOOST_MSVC >= 1400))
#     define AUTOBOOST_REGEX_RECURSIVE
#  else
#     define AUTOBOOST_REGEX_NON_RECURSIVE
#  endif
#endif

#ifdef AUTOBOOST_REGEX_NON_RECURSIVE
#  ifdef AUTOBOOST_REGEX_RECURSIVE
#     error "Can't set both AUTOBOOST_REGEX_RECURSIVE and AUTOBOOST_REGEX_NON_RECURSIVE"
#  endif
#  ifndef AUTOBOOST_REGEX_BLOCKSIZE
#     define AUTOBOOST_REGEX_BLOCKSIZE 4096
#  endif
#  if AUTOBOOST_REGEX_BLOCKSIZE < 512
#     error "AUTOBOOST_REGEX_BLOCKSIZE must be at least 512"
#  endif
#  ifndef AUTOBOOST_REGEX_MAX_BLOCKS
#     define AUTOBOOST_REGEX_MAX_BLOCKS 1024
#  endif
#  ifdef AUTOBOOST_REGEX_HAS_MS_STACK_GUARD
#     undef AUTOBOOST_REGEX_HAS_MS_STACK_GUARD
#  endif
#  ifndef AUTOBOOST_REGEX_MAX_CACHE_BLOCKS
#     define AUTOBOOST_REGEX_MAX_CACHE_BLOCKS 16
#  endif
#endif


/*****************************************************************************
 *
 *  helper memory allocation functions:
 *
 ****************************************************************************/

#if defined(__cplusplus) && defined(AUTOBOOST_REGEX_NON_RECURSIVE)
namespace autoboost{ namespace re_detail{

AUTOBOOST_REGEX_DECL void* AUTOBOOST_REGEX_CALL get_mem_block();
AUTOBOOST_REGEX_DECL void AUTOBOOST_REGEX_CALL put_mem_block(void*);

}} /* namespaces */
#endif

/*****************************************************************************
 *
 *  Diagnostics:
 *
 ****************************************************************************/

#ifdef AUTOBOOST_REGEX_CONFIG_INFO
AUTOBOOST_REGEX_DECL void AUTOBOOST_REGEX_CALL print_regex_library_info();
#endif

#if defined(AUTOBOOST_REGEX_DIAG)
#  pragma message ("AUTOBOOST_REGEX_DECL" AUTOBOOST_STRINGIZE(=AUTOBOOST_REGEX_DECL))
#  pragma message ("AUTOBOOST_REGEX_CALL" AUTOBOOST_STRINGIZE(=AUTOBOOST_REGEX_CALL))
#  pragma message ("AUTOBOOST_REGEX_CCALL" AUTOBOOST_STRINGIZE(=AUTOBOOST_REGEX_CCALL))
#ifdef AUTOBOOST_REGEX_USE_C_LOCALE
#  pragma message ("Using C locale in regex traits class")
#elif AUTOBOOST_REGEX_USE_CPP_LOCALE
#  pragma message ("Using C++ locale in regex traits class")
#else
#  pragma message ("Using Win32 locale in regex traits class")
#endif
#if defined(AUTOBOOST_REGEX_DYN_LINK) || defined(AUTOBOOST_ALL_DYN_LINK)
#  pragma message ("Dynamic linking enabled")
#endif
#if defined(AUTOBOOST_REGEX_NO_LIB) || defined(AUTOBOOST_ALL_NO_LIB)
#  pragma message ("Auto-linking disabled")
#endif
#ifdef AUTOBOOST_REGEX_NO_EXTERNAL_TEMPLATES
#  pragma message ("Extern templates disabled")
#endif

#endif

#endif




