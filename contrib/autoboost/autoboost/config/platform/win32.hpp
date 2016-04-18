//  (C) Copyright John Maddock 2001 - 2003.
//  (C) Copyright Bill Kempf 2001.
//  (C) Copyright Aleksey Gurtovoy 2003.
//  (C) Copyright Rene Rivera 2005.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  Win32 specific config options:

#define AUTOBOOST_PLATFORM "Win32"

//  Get the information about the MinGW runtime, i.e. __MINGW32_*VERSION.
#if defined(__MINGW32__)
#  include <_mingw.h>
#endif

#if defined(__GNUC__) && !defined(AUTOBOOST_NO_SWPRINTF)
#  define AUTOBOOST_NO_SWPRINTF
#endif

//  Default defines for AUTOBOOST_SYMBOL_EXPORT and AUTOBOOST_SYMBOL_IMPORT
//  If a compiler doesn't support __declspec(dllexport)/__declspec(dllimport),
//  its autoboost/config/compiler/ file must define AUTOBOOST_SYMBOL_EXPORT and
//  AUTOBOOST_SYMBOL_IMPORT
#ifndef AUTOBOOST_SYMBOL_EXPORT
#  define AUTOBOOST_HAS_DECLSPEC
#  define AUTOBOOST_SYMBOL_EXPORT __declspec(dllexport)
#  define AUTOBOOST_SYMBOL_IMPORT __declspec(dllimport)
#endif

#if defined(__MINGW32__) && ((__MINGW32_MAJOR_VERSION > 2) || ((__MINGW32_MAJOR_VERSION == 2) && (__MINGW32_MINOR_VERSION >= 0)))
#  define AUTOBOOST_HAS_STDINT_H
#  ifndef __STDC_LIMIT_MACROS
#     define __STDC_LIMIT_MACROS
#  endif
#  define AUTOBOOST_HAS_DIRENT_H
#  define AUTOBOOST_HAS_UNISTD_H
#endif

#if defined(__MINGW32__) && (__GNUC__ >= 4)
// Mingw has these functions but there are persistent problems
// with calls to these crashing, so disable for now:
//#  define AUTOBOOST_HAS_EXPM1
//#  define AUTOBOOST_HAS_LOG1P
#  define AUTOBOOST_HAS_GETTIMEOFDAY
#endif
//
// Win32 will normally be using native Win32 threads,
// but there is a pthread library avaliable as an option,
// we used to disable this when AUTOBOOST_DISABLE_WIN32 was
// defined but no longer - this should allow some
// files to be compiled in strict mode - while maintaining
// a consistent setting of AUTOBOOST_HAS_THREADS across
// all translation units (needed for shared_ptr etc).
//

#ifndef AUTOBOOST_HAS_PTHREADS
#  define AUTOBOOST_HAS_WINTHREADS
#endif

//
// WinCE configuration:
//
#if defined(_WIN32_WCE) || defined(UNDER_CE)
#  define AUTOBOOST_NO_ANSI_APIS
// Windows CE does not have a conforming signature for swprintf
#  define AUTOBOOST_NO_SWPRINTF
#else
#  define AUTOBOOST_HAS_GETSYSTEMTIMEASFILETIME
#  define AUTOBOOST_HAS_THREADEX
#  define AUTOBOOST_HAS_GETSYSTEMTIMEASFILETIME
#endif

//
// Windows Runtime
//
#if defined(WINAPI_FAMILY) && \
  (WINAPI_FAMILY == WINAPI_FAMILY_APP || WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
#  define AUTOBOOST_NO_ANSI_APIS
#endif

#ifndef AUTOBOOST_DISABLE_WIN32
// WEK: Added
#define AUTOBOOST_HAS_FTIME
#define AUTOBOOST_WINDOWS 1

#endif
