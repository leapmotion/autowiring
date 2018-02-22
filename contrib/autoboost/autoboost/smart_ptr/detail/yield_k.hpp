#ifndef AUTOBOOST_SMART_PTR_DETAIL_YIELD_K_HPP_INCLUDED
#define AUTOBOOST_SMART_PTR_DETAIL_YIELD_K_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  yield_k.hpp
//
//  Copyright (c) 2008 Peter Dimov
//  Copyright (c) Microsoft Corporation 2014
//
//  void yield( unsigned k );
//
//  Typical use:
//
//  for( unsigned k = 0; !try_lock(); ++k ) yield( k );
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//

#include <autoboost/config.hpp>
#include <autoboost/predef.h>

#if AUTOBOOST_PLAT_WINDOWS_RUNTIME
#include <thread>
#endif

// AUTOBOOST_SMT_PAUSE

#if defined(_MSC_VER) && _MSC_VER >= 1310 && ( defined(_M_IX86) || defined(_M_X64) )

extern "C" void _mm_pause();

#define AUTOBOOST_SMT_PAUSE _mm_pause();

#elif defined(__GNUC__) && ( defined(__i386__) || defined(__x86_64__) )

#define AUTOBOOST_SMT_PAUSE __asm__ __volatile__( "rep; nop" : : : "memory" );

#endif

//

#if defined( WIN32 ) || defined( _WIN32 ) || defined( __WIN32__ ) || defined( __CYGWIN__ )

#if defined( AUTOBOOST_USE_WINDOWS_H )
# include <windows.h>
#endif

namespace autoboost
{

namespace detail
{

#if !defined( AUTOBOOST_USE_WINDOWS_H ) && !AUTOBOOST_PLAT_WINDOWS_RUNTIME
#if !AUTOBOOST_COMP_CLANG || !defined __MINGW32__
  extern "C" void __stdcall Sleep( unsigned long ms );
#else
#include <_mingw.h>
#if !defined __MINGW64_VERSION_MAJOR
  extern "C" void __stdcall Sleep( unsigned long ms );
#else
  extern "C" __declspec(dllimport) void __stdcall Sleep( unsigned long ms );
#endif
#endif
#endif

inline void yield( unsigned k )
{
    if( k < 4 )
    {
    }
#if defined( AUTOBOOST_SMT_PAUSE )
    else if( k < 16 )
    {
        AUTOBOOST_SMT_PAUSE
    }
#endif
#if !AUTOBOOST_PLAT_WINDOWS_RUNTIME
    else if( k < 32 )
    {
        Sleep( 0 );
    }
    else
    {
        Sleep( 1 );
    }
#else
    else
    {
        // Sleep isn't supported on the Windows Runtime.
        std::this_thread::yield();
    }
#endif
}

} // namespace detail

} // namespace autoboost

#elif defined( AUTOBOOST_HAS_PTHREADS )

#ifndef _AIX
#include <sched.h>
#else
   // AIX's sched.h defines ::var which sometimes conflicts with Lambda's var
       extern "C" int sched_yield(void);
#endif

#include <time.h>

namespace autoboost
{

namespace detail
{

inline void yield( unsigned k )
{
    if( k < 4 )
    {
    }
#if defined( AUTOBOOST_SMT_PAUSE )
    else if( k < 16 )
    {
        AUTOBOOST_SMT_PAUSE
    }
#endif
    else if( k < 32 || k & 1 )
    {
        sched_yield();
    }
    else
    {
        // g++ -Wextra warns on {} or {0}
        struct timespec rqtp = { 0, 0 };

        // POSIX says that timespec has tv_sec and tv_nsec
        // But it doesn't guarantee order or placement

        rqtp.tv_sec = 0;
        rqtp.tv_nsec = 1000;

        nanosleep( &rqtp, 0 );
    }
}

} // namespace detail

} // namespace autoboost

#else

namespace autoboost
{

namespace detail
{

inline void yield( unsigned )
{
}

} // namespace detail

} // namespace autoboost

#endif

#endif // #ifndef AUTOBOOST_SMART_PTR_DETAIL_YIELD_K_HPP_INCLUDED
