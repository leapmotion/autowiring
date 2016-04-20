//  time.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright (c) Microsoft Corporation 2014

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_TIME_HPP
#define AUTOBOOST_DETAIL_WINAPI_TIME_HPP

#include <autoboost/detail/winapi/basic_types.hpp>
#include <autoboost/predef.h>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace autoboost {
namespace detail {
namespace winapi {

#if defined( AUTOBOOST_USE_WINDOWS_H )

    typedef FILETIME FILETIME_;
    typedef PFILETIME PFILETIME_;
    typedef LPFILETIME LPFILETIME_;

    typedef SYSTEMTIME SYSTEMTIME_;
    typedef SYSTEMTIME* PSYSTEMTIME_;

    #ifdef AUTOBOOST_HAS_GETSYSTEMTIMEASFILETIME  // Windows CE does not define GetSystemTimeAsFileTime
    using ::GetSystemTimeAsFileTime;
    #endif
    #if AUTOBOOST_PLAT_WINDOWS_DESKTOP
    using ::FileTimeToLocalFileTime;
    #endif
    using ::GetSystemTime;
    using ::SystemTimeToFileTime;

    #if AUTOBOOST_PLAT_WINDOWS_DESKTOP
    using ::GetTickCount;
    #endif
    #if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
    using ::GetTickCount64;
    #endif

#else

extern "C" {
    typedef struct _FILETIME {
        DWORD_ dwLowDateTime;
        DWORD_ dwHighDateTime;
    } FILETIME_, *PFILETIME_, *LPFILETIME_;

    typedef struct _SYSTEMTIME {
      WORD_ wYear;
      WORD_ wMonth;
      WORD_ wDayOfWeek;
      WORD_ wDay;
      WORD_ wHour;
      WORD_ wMinute;
      WORD_ wSecond;
      WORD_ wMilliseconds;
    } SYSTEMTIME_, *PSYSTEMTIME_;

    #ifdef AUTOBOOST_HAS_GETSYSTEMTIMEASFILETIME  // Windows CE does not define GetSystemTimeAsFileTime
    __declspec(dllimport) void WINAPI
        GetSystemTimeAsFileTime(FILETIME_* lpFileTime);
    #endif
    __declspec(dllimport) int WINAPI
        FileTimeToLocalFileTime(const FILETIME_* lpFileTime,
                FILETIME_* lpLocalFileTime);
    __declspec(dllimport) void WINAPI
        GetSystemTime(SYSTEMTIME_* lpSystemTime);
    __declspec(dllimport) int WINAPI
        SystemTimeToFileTime(const SYSTEMTIME_* lpSystemTime,
                FILETIME_* lpFileTime);
    #if AUTOBOOST_PLAT_WINDOWS_DESKTOP
    __declspec(dllimport) DWORD_ WINAPI
        GetTickCount();
    #endif
    #if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
    __declspec(dllimport) ULONGLONG_ WINAPI
        GetTickCount64();
    #endif
}

#endif

#ifndef AUTOBOOST_HAS_GETSYSTEMTIMEASFILETIME
inline void WINAPI GetSystemTimeAsFileTime(FILETIME_* lpFileTime)
{
    SYSTEMTIME_ st;
    GetSystemTime(&st);
    SystemTimeToFileTime(&st, lpFileTime);
}
#endif

}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_TIME_HPP
