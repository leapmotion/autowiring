//  autoboost/chrono/config.hpp  -------------------------------------------------//

//  Copyright Beman Dawes 2003, 2006, 2008
//  Copyright 2009-2011 Vicente J. Botet Escriba
//  Copyright (c) Microsoft Corporation 2014

//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/chrono for documentation.

#ifndef AUTOBOOST_CHRONO_CONFIG_HPP
#define AUTOBOOST_CHRONO_CONFIG_HPP

#include <autoboost/config.hpp>
#include <autoboost/predef.h>

#if !defined AUTOBOOST_CHRONO_VERSION
#define AUTOBOOST_CHRONO_VERSION 1
#else
#if AUTOBOOST_CHRONO_VERSION!=1  && AUTOBOOST_CHRONO_VERSION!=2
#error "AUTOBOOST_CHRONO_VERSION must be 1 or 2"
#endif
#endif

#if defined(AUTOBOOST_CHRONO_SOURCE) && !defined(AUTOBOOST_USE_WINDOWS_H)
#define AUTOBOOST_USE_WINDOWS_H
#endif

#if ! defined AUTOBOOST_CHRONO_PROVIDES_DATE_IO_FOR_SYSTEM_CLOCK_TIME_POINT \
    && ! defined AUTOBOOST_CHRONO_DONT_PROVIDE_DATE_IO_FOR_SYSTEM_CLOCK_TIME_POINT

# define AUTOBOOST_CHRONO_PROVIDES_DATE_IO_FOR_SYSTEM_CLOCK_TIME_POINT

#endif

//  AUTOBOOST_CHRONO_POSIX_API, AUTOBOOST_CHRONO_MAC_API, or AUTOBOOST_CHRONO_WINDOWS_API
//  can be defined by the user to specify which API should be used

#if defined(AUTOBOOST_CHRONO_WINDOWS_API)
# warning Boost.Chrono will use the Windows API
#elif defined(AUTOBOOST_CHRONO_MAC_API)
# warning Boost.Chrono will use the Mac API
#elif defined(AUTOBOOST_CHRONO_POSIX_API)
# warning Boost.Chrono will use the POSIX API
#endif

# if defined( AUTOBOOST_CHRONO_WINDOWS_API ) && defined( AUTOBOOST_CHRONO_POSIX_API )
#   error both AUTOBOOST_CHRONO_WINDOWS_API and AUTOBOOST_CHRONO_POSIX_API are defined
# elif defined( AUTOBOOST_CHRONO_WINDOWS_API ) && defined( AUTOBOOST_CHRONO_MAC_API )
#   error both AUTOBOOST_CHRONO_WINDOWS_API and AUTOBOOST_CHRONO_MAC_API are defined
# elif defined( AUTOBOOST_CHRONO_MAC_API ) && defined( AUTOBOOST_CHRONO_POSIX_API )
#   error both AUTOBOOST_CHRONO_MAC_API and AUTOBOOST_CHRONO_POSIX_API are defined
# elif !defined( AUTOBOOST_CHRONO_WINDOWS_API ) && !defined( AUTOBOOST_CHRONO_MAC_API ) && !defined( AUTOBOOST_CHRONO_POSIX_API )
#   if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32))
#     define AUTOBOOST_CHRONO_WINDOWS_API
#   elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#     define AUTOBOOST_CHRONO_MAC_API
#   else
#     define AUTOBOOST_CHRONO_POSIX_API
#   endif
# endif

# if defined( AUTOBOOST_CHRONO_WINDOWS_API )
#   ifndef UNDER_CE
#     define AUTOBOOST_CHRONO_HAS_PROCESS_CLOCKS
#   endif
#   define AUTOBOOST_CHRONO_HAS_CLOCK_STEADY
#   if AUTOBOOST_PLAT_WINDOWS_DESKTOP
#     define AUTOBOOST_CHRONO_HAS_THREAD_CLOCK
#   endif
#   define AUTOBOOST_CHRONO_THREAD_CLOCK_IS_STEADY true
# endif

# if defined( AUTOBOOST_CHRONO_MAC_API )
#   define AUTOBOOST_CHRONO_HAS_PROCESS_CLOCKS
#   define AUTOBOOST_CHRONO_HAS_CLOCK_STEADY
#   define AUTOBOOST_CHRONO_HAS_THREAD_CLOCK
#   define AUTOBOOST_CHRONO_THREAD_CLOCK_IS_STEADY true
# endif

# if defined( AUTOBOOST_CHRONO_POSIX_API )
#   define AUTOBOOST_CHRONO_HAS_PROCESS_CLOCKS
#   include <time.h>  //to check for CLOCK_REALTIME and CLOCK_MONOTONIC and _POSIX_THREAD_CPUTIME
#   if defined(CLOCK_MONOTONIC)
#      define AUTOBOOST_CHRONO_HAS_CLOCK_STEADY
#   endif
#   if defined(_POSIX_THREAD_CPUTIME) && !defined(AUTOBOOST_DISABLE_THREADS)
#     define AUTOBOOST_CHRONO_HAS_THREAD_CLOCK
#     define AUTOBOOST_CHRONO_THREAD_CLOCK_IS_STEADY true
#   endif
#   if defined(CLOCK_THREAD_CPUTIME_ID) && !defined(AUTOBOOST_DISABLE_THREADS)
#     define AUTOBOOST_CHRONO_HAS_THREAD_CLOCK
#     define AUTOBOOST_CHRONO_THREAD_CLOCK_IS_STEADY true
#   endif
#   if defined(sun) || defined(__sun)
#     undef AUTOBOOST_CHRONO_HAS_THREAD_CLOCK
#     undef AUTOBOOST_CHRONO_THREAD_CLOCK_IS_STEADY
#   endif
#   if (defined(__HP_aCC) || defined(__GNUC__)) && defined(__hpux)
#     undef AUTOBOOST_CHRONO_HAS_THREAD_CLOCK
#     undef AUTOBOOST_CHRONO_THREAD_CLOCK_IS_STEADY
#   endif
#   if defined(__VXWORKS__)
#     undef AUTOBOOST_CHRONO_HAS_PROCESS_CLOCKS
#   endif
# endif

#if defined(AUTOBOOST_CHRONO_THREAD_DISABLED) && defined(AUTOBOOST_CHRONO_HAS_THREAD_CLOCK)
#undef AUTOBOOST_CHRONO_HAS_THREAD_CLOCK
#undef AUTOBOOST_CHRONO_THREAD_CLOCK_IS_STEADY
#endif

// unicode support  ------------------------------//

#if defined(AUTOBOOST_NO_CXX11_UNICODE_LITERALS) || defined(AUTOBOOST_NO_CXX11_CHAR16_T) || defined(AUTOBOOST_NO_CXX11_CHAR32_T)
//~ #define AUTOBOOST_CHRONO_HAS_UNICODE_SUPPORT
#else
#define AUTOBOOST_CHRONO_HAS_UNICODE_SUPPORT 1
#endif

#ifndef AUTOBOOST_CHRONO_LIB_CONSTEXPR
#if defined( AUTOBOOST_NO_CXX11_NUMERIC_LIMITS )
#define AUTOBOOST_CHRONO_LIB_CONSTEXPR
#elif defined(_LIBCPP_VERSION) &&  !defined(_LIBCPP_CONSTEXPR)
  #define AUTOBOOST_CHRONO_LIB_CONSTEXPR
#else
  #define AUTOBOOST_CHRONO_LIB_CONSTEXPR AUTOBOOST_CONSTEXPR
#endif
#endif

#if defined( AUTOBOOST_NO_CXX11_NUMERIC_LIMITS )
#  define AUTOBOOST_CHRONO_LIB_NOEXCEPT_OR_THROW throw()
#else
#ifdef AUTOBOOST_NO_CXX11_NOEXCEPT
#  define AUTOBOOST_CHRONO_LIB_NOEXCEPT_OR_THROW throw()
#else
#  define AUTOBOOST_CHRONO_LIB_NOEXCEPT_OR_THROW noexcept
#endif
#endif

#if defined AUTOBOOST_CHRONO_PROVIDE_HYBRID_ERROR_HANDLING \
 && defined AUTOBOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING
#error "AUTOBOOST_CHRONO_PROVIDE_HYBRID_ERROR_HANDLING && AUTOBOOST_CHRONO_PROVIDE_HYBRID_ERROR_HANDLING defined"
#endif

#if defined AUTOBOOST_CHRONO_PROVIDES_DEPRECATED_IO_SINCE_V2_0_0 \
 && defined AUTOBOOST_CHRONO_DONT_PROVIDES_DEPRECATED_IO_SINCE_V2_0_0
#error "AUTOBOOST_CHRONO_PROVIDES_DEPRECATED_IO_SINCE_V2_0_0 && AUTOBOOST_CHRONO_DONT_PROVIDES_DEPRECATED_IO_SINCE_V2_0_0 defined"
#endif

#if ! defined AUTOBOOST_CHRONO_PROVIDE_HYBRID_ERROR_HANDLING \
 && ! defined AUTOBOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING
#define AUTOBOOST_CHRONO_PROVIDE_HYBRID_ERROR_HANDLING
#endif

#if (AUTOBOOST_CHRONO_VERSION == 2)
#if ! defined AUTOBOOST_CHRONO_PROVIDES_DEPRECATED_IO_SINCE_V2_0_0 \
 && ! defined AUTOBOOST_CHRONO_DONT_PROVIDES_DEPRECATED_IO_SINCE_V2_0_0
#define AUTOBOOST_CHRONO_DONT_PROVIDES_DEPRECATED_IO_SINCE_V2_0_0
#endif
#endif

#ifdef AUTOBOOST_CHRONO_HEADER_ONLY
#define AUTOBOOST_CHRONO_INLINE inline
#define AUTOBOOST_CHRONO_STATIC inline
#define AUTOBOOST_CHRONO_DECL

#else
#define AUTOBOOST_CHRONO_INLINE
#define AUTOBOOST_CHRONO_STATIC static

//  enable dynamic linking on Windows  ---------------------------------------//

// we need to import/export our code only if the user has specifically
// asked for it by defining either AUTOBOOST_ALL_DYN_LINK if they want all autoboost
// libraries to be dynamically linked, or AUTOBOOST_CHRONO_DYN_LINK
// if they want just this one to be dynamically liked:
#if defined(AUTOBOOST_ALL_DYN_LINK) || defined(AUTOBOOST_CHRONO_DYN_LINK)
// export if this is our own source, otherwise import:
#ifdef AUTOBOOST_CHRONO_SOURCE
# define AUTOBOOST_CHRONO_DECL AUTOBOOST_SYMBOL_EXPORT
#else
# define AUTOBOOST_CHRONO_DECL AUTOBOOST_SYMBOL_IMPORT
#endif  // AUTOBOOST_CHRONO_SOURCE
#endif  // DYN_LINK
//
// if AUTOBOOST_CHRONO_DECL isn't defined yet define it now:
#ifndef AUTOBOOST_CHRONO_DECL
#define AUTOBOOST_CHRONO_DECL
#endif



//  enable automatic library variant selection  ------------------------------//

#if !defined(AUTOBOOST_CHRONO_SOURCE) && !defined(AUTOBOOST_ALL_NO_LIB) && !defined(AUTOBOOST_CHRONO_NO_LIB)
//
// Set the name of our library; this will get undef'ed by auto_link.hpp
// once it's done with it:
//
#define AUTOBOOST_LIB_NAME autoboost_chrono
//
// If we're importing code from a dll, then tell auto_link.hpp about it:
//
#if defined(AUTOBOOST_ALL_DYN_LINK) || defined(AUTOBOOST_CHRONO_DYN_LINK)
#  define AUTOBOOST_DYN_LINK
#endif
//
// And include the header that does the work:
//
#include <autoboost/config/auto_link.hpp>
#endif  // auto-linking disabled
#endif // AUTOBOOST_CHRONO_HEADER_ONLY
#endif // AUTOBOOST_CHRONO_CONFIG_HPP

