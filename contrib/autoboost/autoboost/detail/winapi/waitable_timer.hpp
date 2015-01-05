//  waitable_timer.hpp  --------------------------------------------------------------//

//  Copyright 2013 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_WAITABLE_TIMER_HPP
#define AUTOBOOST_DETAIL_WINAPI_WAITABLE_TIMER_HPP

#include <autoboost/detail/winapi/basic_types.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace autoboost
{
namespace detail
{
namespace winapi
{
#if defined( AUTOBOOST_USE_WINDOWS_H )

typedef ::PTIMERAPCROUTINE PTIMERAPCROUTINE_;

# ifdef AUTOBOOST_NO_ANSI_APIS
using ::CreateWaitableTimerW;
using ::OpenWaitableTimerW;
# else
using ::CreateWaitableTimerA;
using ::OpenWaitableTimerA;
# endif
using ::SetWaitableTimer;
using ::CancelWaitableTimer;

#else

extern "C" {

struct _SECURITY_ATTRIBUTES;

typedef void (WINAPI* PTIMERAPCROUTINE_)
(
    LPVOID_ lpArgToCompletionRoutine,
    DWORD_ dwTimerLowValue,
    DWORD_ dwTimerHighValue
);

# ifdef AUTOBOOST_NO_ANSI_APIS
__declspec(dllimport) HANDLE_ WINAPI CreateWaitableTimerW
(
    _SECURITY_ATTRIBUTES* lpTimerAttributes,
    BOOL_ bManualReset,
    LPCWSTR_ lpTimerName
);

__declspec(dllimport) HANDLE_ WINAPI OpenWaitableTimerW
(
    DWORD_ dwDesiredAccess,
    BOOL_ bInheritHandle,
    LPCWSTR_ lpTimerName
);
# else
__declspec(dllimport) HANDLE_ WINAPI CreateWaitableTimerA
(
    _SECURITY_ATTRIBUTES* lpTimerAttributes,
    BOOL_ bManualReset,
    LPCSTR_ lpTimerName
);

__declspec(dllimport) HANDLE_ WINAPI OpenWaitableTimerA
(
    DWORD_ dwDesiredAccess,
    BOOL_ bInheritHandle,
    LPCSTR_ lpTimerName
);
# endif

__declspec(dllimport) BOOL_ WINAPI SetWaitableTimer
(
    HANDLE_ hTimer,
    const LARGE_INTEGER_ *lpDueTime,
    LONG_ lPeriod,
    PTIMERAPCROUTINE_ pfnCompletionRoutine,
    LPVOID_ lpArgToCompletionRoutine,
    BOOL_ fResume
);

__declspec(dllimport) BOOL_ WINAPI CancelWaitableTimer(HANDLE_ hTimer);

}

#endif

AUTOBOOST_FORCEINLINE HANDLE_ create_anonymous_waitable_timer(_SECURITY_ATTRIBUTES* lpTimerAttributes, BOOL_ bManualReset)
{
#ifdef AUTOBOOST_NO_ANSI_APIS
    return CreateWaitableTimerW(lpTimerAttributes, bManualReset, 0);
#else
    return CreateWaitableTimerA(lpTimerAttributes, bManualReset, 0);
#endif
}

}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_WAITABLE_TIMER_HPP
