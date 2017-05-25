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

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
typedef autoboost::detail::winapi::VOID_
(WINAPI *PTIMERAPCROUTINE)(
    autoboost::detail::winapi::LPVOID_ lpArgToCompletionRoutine,
    autoboost::detail::winapi::DWORD_ dwTimerLowValue,
    autoboost::detail::winapi::DWORD_ dwTimerHighValue);

#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
CreateWaitableTimerA(
    ::_SECURITY_ATTRIBUTES* lpTimerAttributes,
    autoboost::detail::winapi::BOOL_ bManualReset,
    autoboost::detail::winapi::LPCSTR_ lpTimerName);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
OpenWaitableTimerA(
    autoboost::detail::winapi::DWORD_ dwDesiredAccess,
    autoboost::detail::winapi::BOOL_ bInheritHandle,
    autoboost::detail::winapi::LPCSTR_ lpTimerName);
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
CreateWaitableTimerW(
    ::_SECURITY_ATTRIBUTES* lpTimerAttributes,
    autoboost::detail::winapi::BOOL_ bManualReset,
    autoboost::detail::winapi::LPCWSTR_ lpTimerName);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
OpenWaitableTimerW(
    autoboost::detail::winapi::DWORD_ dwDesiredAccess,
    autoboost::detail::winapi::BOOL_ bInheritHandle,
    autoboost::detail::winapi::LPCWSTR_ lpTimerName);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
SetWaitableTimer(
    autoboost::detail::winapi::HANDLE_ hTimer,
    const ::_LARGE_INTEGER* lpDueTime,
    autoboost::detail::winapi::LONG_ lPeriod,
    PTIMERAPCROUTINE pfnCompletionRoutine,
    autoboost::detail::winapi::LPVOID_ lpArgToCompletionRoutine,
    autoboost::detail::winapi::BOOL_ fResume);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
CancelWaitableTimer(autoboost::detail::winapi::HANDLE_ hTimer);
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {

typedef ::PTIMERAPCROUTINE PTIMERAPCROUTINE_;

#if !defined( AUTOBOOST_NO_ANSI_APIS )
using ::OpenWaitableTimerA;
#endif
using ::OpenWaitableTimerW;
using ::CancelWaitableTimer;

#if defined( AUTOBOOST_USE_WINDOWS_H )

const DWORD_ TIMER_ALL_ACCESS_ = TIMER_ALL_ACCESS;
const DWORD_ TIMER_MODIFY_STATE_ = TIMER_MODIFY_STATE;
const DWORD_ TIMER_QUERY_STATE_ = TIMER_QUERY_STATE;

#else // defined( AUTOBOOST_USE_WINDOWS_H )

const DWORD_ TIMER_ALL_ACCESS_ = 0x001F0003;
const DWORD_ TIMER_MODIFY_STATE_ = 0x00000002;
const DWORD_ TIMER_QUERY_STATE_ = 0x00000001;

#endif // defined( AUTOBOOST_USE_WINDOWS_H )

const DWORD_ timer_all_access = TIMER_ALL_ACCESS_;
const DWORD_ timer_modify_state = TIMER_MODIFY_STATE_;
const DWORD_ timer_query_state = TIMER_QUERY_STATE_;


#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_FORCEINLINE HANDLE_ CreateWaitableTimerA(PSECURITY_ATTRIBUTES_ lpTimerAttributes, BOOL_ bManualReset, LPCSTR_ lpTimerName)
{
    return ::CreateWaitableTimerA(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpTimerAttributes), bManualReset, lpTimerName);
}
#endif

AUTOBOOST_FORCEINLINE HANDLE_ CreateWaitableTimerW(PSECURITY_ATTRIBUTES_ lpTimerAttributes, BOOL_ bManualReset, LPCWSTR_ lpTimerName)
{
    return ::CreateWaitableTimerW(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpTimerAttributes), bManualReset, lpTimerName);
}

AUTOBOOST_FORCEINLINE BOOL_ SetWaitableTimer(
    HANDLE_ hTimer,
    const LARGE_INTEGER_* lpDueTime,
    LONG_ lPeriod,
    PTIMERAPCROUTINE_ pfnCompletionRoutine,
    LPVOID_ lpArgToCompletionRoutine,
    BOOL_ fResume)
{
    return ::SetWaitableTimer(hTimer, reinterpret_cast< const ::_LARGE_INTEGER* >(lpDueTime), lPeriod, pfnCompletionRoutine, lpArgToCompletionRoutine, fResume);
}

#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_FORCEINLINE HANDLE_ create_waitable_timer(PSECURITY_ATTRIBUTES_ lpTimerAttributes, BOOL_ bManualReset, LPCSTR_ lpTimerName)
{
    return ::CreateWaitableTimerA(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpTimerAttributes), bManualReset, lpTimerName);
}
#endif

AUTOBOOST_FORCEINLINE HANDLE_ create_waitable_timer(PSECURITY_ATTRIBUTES_ lpTimerAttributes, BOOL_ bManualReset, LPCWSTR_ lpTimerName)
{
    return ::CreateWaitableTimerW(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpTimerAttributes), bManualReset, lpTimerName);
}

AUTOBOOST_FORCEINLINE HANDLE_ create_anonymous_waitable_timer(PSECURITY_ATTRIBUTES_ lpTimerAttributes, BOOL_ bManualReset)
{
#ifdef AUTOBOOST_NO_ANSI_APIS
    return ::CreateWaitableTimerW(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpTimerAttributes), bManualReset, 0);
#else
    return ::CreateWaitableTimerA(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpTimerAttributes), bManualReset, 0);
#endif
}

}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_WAITABLE_TIMER_HPP
