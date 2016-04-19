#ifndef AUTOBOOST_DETAIL_INTERLOCKED_HPP_INCLUDED
#define AUTOBOOST_DETAIL_INTERLOCKED_HPP_INCLUDED

//
//  autoboost/detail/interlocked.hpp
//
//  Copyright 2005 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#include <autoboost/config.hpp>

// MS compatible compilers support #pragma once
#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if defined( AUTOBOOST_USE_WINDOWS_H )

# include <windows.h>

# define AUTOBOOST_INTERLOCKED_INCREMENT InterlockedIncrement
# define AUTOBOOST_INTERLOCKED_DECREMENT InterlockedDecrement
# define AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE InterlockedCompareExchange
# define AUTOBOOST_INTERLOCKED_EXCHANGE InterlockedExchange
# define AUTOBOOST_INTERLOCKED_EXCHANGE_ADD InterlockedExchangeAdd
# define AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE_POINTER InterlockedCompareExchangePointer
# define AUTOBOOST_INTERLOCKED_EXCHANGE_POINTER InterlockedExchangePointer

#elif defined( AUTOBOOST_USE_INTRIN_H )

#include <intrin.h>

# define AUTOBOOST_INTERLOCKED_INCREMENT _InterlockedIncrement
# define AUTOBOOST_INTERLOCKED_DECREMENT _InterlockedDecrement
# define AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE _InterlockedCompareExchange
# define AUTOBOOST_INTERLOCKED_EXCHANGE _InterlockedExchange
# define AUTOBOOST_INTERLOCKED_EXCHANGE_ADD _InterlockedExchangeAdd

# if defined(_M_IA64) || defined(_M_AMD64) || defined(__x86_64__) || defined(__x86_64)

#  define AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE_POINTER _InterlockedCompareExchangePointer
#  define AUTOBOOST_INTERLOCKED_EXCHANGE_POINTER _InterlockedExchangePointer

# else

#  define AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE_POINTER(dest,exchange,compare) \
    ((void*)AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE((long volatile*)(dest),(long)(exchange),(long)(compare)))
#  define AUTOBOOST_INTERLOCKED_EXCHANGE_POINTER(dest,exchange) \
    ((void*)AUTOBOOST_INTERLOCKED_EXCHANGE((long volatile*)(dest),(long)(exchange)))

# endif

#elif defined(_WIN32_WCE)

#if _WIN32_WCE >= 0x600

extern "C" long __cdecl _InterlockedIncrement( long volatile * );
extern "C" long __cdecl _InterlockedDecrement( long volatile * );
extern "C" long __cdecl _InterlockedCompareExchange( long volatile *, long, long );
extern "C" long __cdecl _InterlockedExchange( long volatile *, long );
extern "C" long __cdecl _InterlockedExchangeAdd( long volatile *, long );

# define AUTOBOOST_INTERLOCKED_INCREMENT _InterlockedIncrement
# define AUTOBOOST_INTERLOCKED_DECREMENT _InterlockedDecrement
# define AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE _InterlockedCompareExchange
# define AUTOBOOST_INTERLOCKED_EXCHANGE _InterlockedExchange
# define AUTOBOOST_INTERLOCKED_EXCHANGE_ADD _InterlockedExchangeAdd

#else
// under Windows CE we still have old-style Interlocked* functions

extern "C" long __cdecl InterlockedIncrement( long* );
extern "C" long __cdecl InterlockedDecrement( long* );
extern "C" long __cdecl InterlockedCompareExchange( long*, long, long );
extern "C" long __cdecl InterlockedExchange( long*, long );
extern "C" long __cdecl InterlockedExchangeAdd( long*, long );

# define AUTOBOOST_INTERLOCKED_INCREMENT InterlockedIncrement
# define AUTOBOOST_INTERLOCKED_DECREMENT InterlockedDecrement
# define AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE InterlockedCompareExchange
# define AUTOBOOST_INTERLOCKED_EXCHANGE InterlockedExchange
# define AUTOBOOST_INTERLOCKED_EXCHANGE_ADD InterlockedExchangeAdd

#endif

# define AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE_POINTER(dest,exchange,compare) \
    ((void*)AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE((long*)(dest),(long)(exchange),(long)(compare)))
# define AUTOBOOST_INTERLOCKED_EXCHANGE_POINTER(dest,exchange) \
    ((void*)AUTOBOOST_INTERLOCKED_EXCHANGE((long*)(dest),(long)(exchange)))

#elif defined( AUTOBOOST_MSVC ) || defined( AUTOBOOST_INTEL_WIN )

#if defined( AUTOBOOST_MSVC ) && AUTOBOOST_MSVC >= 1400

#include <intrin.h>

#else

# if defined( __CLRCALL_PURE_OR_CDECL )
#  define AUTOBOOST_INTERLOCKED_CLRCALL_PURE_OR_CDECL __CLRCALL_PURE_OR_CDECL
# else
#  define AUTOBOOST_INTERLOCKED_CLRCALL_PURE_OR_CDECL __cdecl
# endif

extern "C" long AUTOBOOST_INTERLOCKED_CLRCALL_PURE_OR_CDECL _InterlockedIncrement( long volatile * );
extern "C" long AUTOBOOST_INTERLOCKED_CLRCALL_PURE_OR_CDECL _InterlockedDecrement( long volatile * );
extern "C" long AUTOBOOST_INTERLOCKED_CLRCALL_PURE_OR_CDECL _InterlockedCompareExchange( long volatile *, long, long );
extern "C" long AUTOBOOST_INTERLOCKED_CLRCALL_PURE_OR_CDECL _InterlockedExchange( long volatile *, long );
extern "C" long AUTOBOOST_INTERLOCKED_CLRCALL_PURE_OR_CDECL _InterlockedExchangeAdd( long volatile *, long );

# undef AUTOBOOST_INTERLOCKED_CLRCALL_PURE_OR_CDECL

# if defined( AUTOBOOST_MSVC ) && AUTOBOOST_MSVC >= 1310
#  pragma intrinsic( _InterlockedIncrement )
#  pragma intrinsic( _InterlockedDecrement )
#  pragma intrinsic( _InterlockedCompareExchange )
#  pragma intrinsic( _InterlockedExchange )
#  pragma intrinsic( _InterlockedExchangeAdd )
# endif

#endif

# if defined(_M_IA64) || defined(_M_AMD64)

extern "C" void* __cdecl _InterlockedCompareExchangePointer( void* volatile *, void*, void* );
extern "C" void* __cdecl _InterlockedExchangePointer( void* volatile *, void* );

#  define AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE_POINTER _InterlockedCompareExchangePointer
#  define AUTOBOOST_INTERLOCKED_EXCHANGE_POINTER _InterlockedExchangePointer

# else

#  define AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE_POINTER(dest,exchange,compare) \
    ((void*)AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE((long volatile*)(dest),(long)(exchange),(long)(compare)))
#  define AUTOBOOST_INTERLOCKED_EXCHANGE_POINTER(dest,exchange) \
    ((void*)AUTOBOOST_INTERLOCKED_EXCHANGE((long volatile*)(dest),(long)(exchange)))

# endif

# define AUTOBOOST_INTERLOCKED_INCREMENT _InterlockedIncrement
# define AUTOBOOST_INTERLOCKED_DECREMENT _InterlockedDecrement
# define AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE _InterlockedCompareExchange
# define AUTOBOOST_INTERLOCKED_EXCHANGE _InterlockedExchange
# define AUTOBOOST_INTERLOCKED_EXCHANGE_ADD _InterlockedExchangeAdd

// Unlike __MINGW64__, __MINGW64_VERSION_MAJOR is defined by MinGW-w64 for both 32 and 64-bit targets.
#elif defined(__MINGW64_VERSION_MAJOR)

// MinGW-w64 provides intrin.h for both 32 and 64-bit targets.
#include <intrin.h>

# define AUTOBOOST_INTERLOCKED_INCREMENT _InterlockedIncrement
# define AUTOBOOST_INTERLOCKED_DECREMENT _InterlockedDecrement
# define AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE _InterlockedCompareExchange
# define AUTOBOOST_INTERLOCKED_EXCHANGE _InterlockedExchange
# define AUTOBOOST_INTERLOCKED_EXCHANGE_ADD _InterlockedExchangeAdd
# if defined(__x86_64__) || defined(__x86_64)
#  define AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE_POINTER _InterlockedCompareExchangePointer
#  define AUTOBOOST_INTERLOCKED_EXCHANGE_POINTER _InterlockedExchangePointer
# else
#  define AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE_POINTER(dest,exchange,compare) \
    ((void*)AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE((long volatile*)(dest),(long)(exchange),(long)(compare)))
#  define AUTOBOOST_INTERLOCKED_EXCHANGE_POINTER(dest,exchange) \
    ((void*)AUTOBOOST_INTERLOCKED_EXCHANGE((long volatile*)(dest),(long)(exchange)))
# endif

#elif defined( WIN32 ) || defined( _WIN32 ) || defined( __WIN32__ ) || defined( __CYGWIN__ )

#define AUTOBOOST_INTERLOCKED_IMPORT __declspec(dllimport)

namespace autoboost
{

namespace detail
{

extern "C" AUTOBOOST_INTERLOCKED_IMPORT long __stdcall InterlockedIncrement( long volatile * );
extern "C" AUTOBOOST_INTERLOCKED_IMPORT long __stdcall InterlockedDecrement( long volatile * );
extern "C" AUTOBOOST_INTERLOCKED_IMPORT long __stdcall InterlockedCompareExchange( long volatile *, long, long );
extern "C" AUTOBOOST_INTERLOCKED_IMPORT long __stdcall InterlockedExchange( long volatile *, long );
extern "C" AUTOBOOST_INTERLOCKED_IMPORT long __stdcall InterlockedExchangeAdd( long volatile *, long );

# if defined(_M_IA64) || defined(_M_AMD64)
extern "C" AUTOBOOST_INTERLOCKED_IMPORT void* __stdcall InterlockedCompareExchangePointer( void* volatile *, void*, void* );
extern "C" AUTOBOOST_INTERLOCKED_IMPORT void* __stdcall InterlockedExchangePointer( void* volatile *, void* );
# endif

} // namespace detail

} // namespace autoboost

# define AUTOBOOST_INTERLOCKED_INCREMENT ::autoboost::detail::InterlockedIncrement
# define AUTOBOOST_INTERLOCKED_DECREMENT ::autoboost::detail::InterlockedDecrement
# define AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE ::autoboost::detail::InterlockedCompareExchange
# define AUTOBOOST_INTERLOCKED_EXCHANGE ::autoboost::detail::InterlockedExchange
# define AUTOBOOST_INTERLOCKED_EXCHANGE_ADD ::autoboost::detail::InterlockedExchangeAdd

# if defined(_M_IA64) || defined(_M_AMD64)
#  define AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE_POINTER ::autoboost::detail::InterlockedCompareExchangePointer
#  define AUTOBOOST_INTERLOCKED_EXCHANGE_POINTER ::autoboost::detail::InterlockedExchangePointer
# else
#  define AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE_POINTER(dest,exchange,compare) \
    ((void*)AUTOBOOST_INTERLOCKED_COMPARE_EXCHANGE((long volatile*)(dest),(long)(exchange),(long)(compare)))
#  define AUTOBOOST_INTERLOCKED_EXCHANGE_POINTER(dest,exchange) \
    ((void*)AUTOBOOST_INTERLOCKED_EXCHANGE((long volatile*)(dest),(long)(exchange)))
# endif

#else

# error "Interlocked intrinsics not available"

#endif

#endif // #ifndef AUTOBOOST_DETAIL_INTERLOCKED_HPP_INCLUDED
