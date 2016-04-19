//  time.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright (c) Microsoft Corporation 2014
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_TIME_HPP
#define AUTOBOOST_DETAIL_WINAPI_TIME_HPP

#include <autoboost/detail/winapi/basic_types.hpp>
#include <autoboost/predef/platform.h>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
struct _FILETIME;
struct _SYSTEMTIME;

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
GetSystemTime(::_SYSTEMTIME* lpSystemTime);

#ifdef AUTOBOOST_HAS_GETSYSTEMTIMEASFILETIME  // Windows CE does not define GetSystemTimeAsFileTime
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
GetSystemTimeAsFileTime(::_FILETIME* lpSystemTimeAsFileTime);
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
SystemTimeToFileTime(
    const ::_SYSTEMTIME* lpSystemTime,
    ::_FILETIME* lpFileTime);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
FileTimeToSystemTime(
    const ::_FILETIME* lpFileTime,
    ::_SYSTEMTIME* lpSystemTime);

#if AUTOBOOST_PLAT_WINDOWS_DESKTOP
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
FileTimeToLocalFileTime(
    const ::_FILETIME* lpFileTime,
    ::_FILETIME* lpLocalFileTime);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
LocalFileTimeToFileTime(
    const ::_FILETIME* lpLocalFileTime,
    ::_FILETIME* lpFileTime);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
GetTickCount(AUTOBOOST_DETAIL_WINAPI_VOID);
#endif

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::ULONGLONG_ WINAPI
GetTickCount64(AUTOBOOST_DETAIL_WINAPI_VOID);
#endif
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {

typedef struct AUTOBOOST_DETAIL_WINAPI_MAY_ALIAS _FILETIME {
    DWORD_ dwLowDateTime;
    DWORD_ dwHighDateTime;
} FILETIME_, *PFILETIME_, *LPFILETIME_;

typedef struct AUTOBOOST_DETAIL_WINAPI_MAY_ALIAS _SYSTEMTIME {
    WORD_ wYear;
    WORD_ wMonth;
    WORD_ wDayOfWeek;
    WORD_ wDay;
    WORD_ wHour;
    WORD_ wMinute;
    WORD_ wSecond;
    WORD_ wMilliseconds;
} SYSTEMTIME_, *PSYSTEMTIME_, *LPSYSTEMTIME_;

#if AUTOBOOST_PLAT_WINDOWS_DESKTOP
using ::GetTickCount;
#endif
#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
using ::GetTickCount64;
#endif

AUTOBOOST_FORCEINLINE VOID_ GetSystemTime(LPSYSTEMTIME_ lpSystemTime)
{
    ::GetSystemTime(reinterpret_cast< ::_SYSTEMTIME* >(lpSystemTime));
}

#if defined( AUTOBOOST_HAS_GETSYSTEMTIMEASFILETIME )
AUTOBOOST_FORCEINLINE VOID_ GetSystemTimeAsFileTime(LPFILETIME_ lpSystemTimeAsFileTime)
{
    ::GetSystemTimeAsFileTime(reinterpret_cast< ::_FILETIME* >(lpSystemTimeAsFileTime));
}
#else
// Windows CE does not define GetSystemTimeAsFileTime
AUTOBOOST_FORCEINLINE VOID_ GetSystemTimeAsFileTime(FILETIME_* lpFileTime)
{
    autoboost::detail::winapi::SYSTEMTIME_ st;
    autoboost::detail::winapi::GetSystemTime(&st);
    autoboost::detail::winapi::SystemTimeToFileTime(&st, lpFileTime);
}
#endif

AUTOBOOST_FORCEINLINE BOOL_ SystemTimeToFileTime(const SYSTEMTIME_* lpSystemTime, FILETIME_* lpFileTime)
{
    return ::SystemTimeToFileTime(reinterpret_cast< const ::_SYSTEMTIME* >(lpSystemTime), reinterpret_cast< ::_FILETIME* >(lpFileTime));
}

AUTOBOOST_FORCEINLINE BOOL_ FileTimeToSystemTime(const FILETIME_* lpFileTime, SYSTEMTIME_* lpSystemTime)
{
    return ::FileTimeToSystemTime(reinterpret_cast< const ::_FILETIME* >(lpFileTime), reinterpret_cast< ::_SYSTEMTIME* >(lpSystemTime));
}

#if AUTOBOOST_PLAT_WINDOWS_DESKTOP
AUTOBOOST_FORCEINLINE BOOL_ FileTimeToLocalFileTime(const FILETIME_* lpFileTime, FILETIME_* lpLocalFileTime)
{
    return ::FileTimeToLocalFileTime(reinterpret_cast< const ::_FILETIME* >(lpFileTime), reinterpret_cast< ::_FILETIME* >(lpLocalFileTime));
}

AUTOBOOST_FORCEINLINE BOOL_ LocalFileTimeToFileTime(const FILETIME_* lpLocalFileTime, FILETIME_* lpFileTime)
{
    return ::LocalFileTimeToFileTime(reinterpret_cast< const ::_FILETIME* >(lpLocalFileTime), reinterpret_cast< ::_FILETIME* >(lpFileTime));
}
#endif

}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_TIME_HPP
