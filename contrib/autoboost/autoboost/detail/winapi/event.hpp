//  event.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_EVENT_HPP
#define AUTOBOOST_DETAIL_WINAPI_EVENT_HPP

#include <autoboost/detail/winapi/basic_types.hpp>
#include <autoboost/predef/platform.h>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
#if !defined( AUTOBOOST_NO_ANSI_APIS )
#if !defined( AUTOBOOST_PLAT_WINDOWS_RUNTIME_AVALIABLE )
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
CreateEventA(
    ::_SECURITY_ATTRIBUTES* lpEventAttributes,
    autoboost::detail::winapi::BOOL_ bManualReset,
    autoboost::detail::winapi::BOOL_ bInitialState,
    autoboost::detail::winapi::LPCSTR_ lpName);
#endif

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
CreateEventExA(
    ::_SECURITY_ATTRIBUTES *lpEventAttributes,
    autoboost::detail::winapi::LPCSTR_ lpName,
    autoboost::detail::winapi::DWORD_ dwFlags,
    autoboost::detail::winapi::DWORD_ dwDesiredAccess);
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
OpenEventA(
    autoboost::detail::winapi::DWORD_ dwDesiredAccess,
    autoboost::detail::winapi::BOOL_ bInheritHandle,
    autoboost::detail::winapi::LPCSTR_ lpName);
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
CreateEventW(
    ::_SECURITY_ATTRIBUTES* lpEventAttributes,
    autoboost::detail::winapi::BOOL_ bManualReset,
    autoboost::detail::winapi::BOOL_ bInitialState,
    autoboost::detail::winapi::LPCWSTR_ lpName);

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
CreateEventExW(
    ::_SECURITY_ATTRIBUTES *lpEventAttributes,
    autoboost::detail::winapi::LPCWSTR_ lpName,
    autoboost::detail::winapi::DWORD_ dwFlags,
    autoboost::detail::winapi::DWORD_ dwDesiredAccess);
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
OpenEventW(
    autoboost::detail::winapi::DWORD_ dwDesiredAccess,
    autoboost::detail::winapi::BOOL_ bInheritHandle,
    autoboost::detail::winapi::LPCWSTR_ lpName);

// Windows CE define SetEvent/ResetEvent as inline functions in kfuncs.h
#if !defined( UNDER_CE )
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
SetEvent(autoboost::detail::winapi::HANDLE_ hEvent);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
ResetEvent(autoboost::detail::winapi::HANDLE_ hEvent);
#endif
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {

#if !defined( AUTOBOOST_NO_ANSI_APIS )
using ::OpenEventA;
#endif
using ::OpenEventW;
using ::SetEvent;
using ::ResetEvent;

#if defined( AUTOBOOST_USE_WINDOWS_H )

const DWORD_ EVENT_ALL_ACCESS_ = EVENT_ALL_ACCESS;
const DWORD_ EVENT_MODIFY_STATE_ = EVENT_MODIFY_STATE;
#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
const DWORD_ CREATE_EVENT_INITIAL_SET_ = CREATE_EVENT_INITIAL_SET;
const DWORD_ CREATE_EVENT_MANUAL_RESET_ = CREATE_EVENT_MANUAL_RESET;
#endif

#else // defined( AUTOBOOST_USE_WINDOWS_H )

const DWORD_ EVENT_ALL_ACCESS_ = 0x001F0003;
const DWORD_ EVENT_MODIFY_STATE_ = 0x00000002;
#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
const DWORD_ CREATE_EVENT_INITIAL_SET_ = 0x00000002;
const DWORD_ CREATE_EVENT_MANUAL_RESET_ = 0x00000001;
#endif

#endif // defined( AUTOBOOST_USE_WINDOWS_H )

// Undocumented and not present in Windows SDK. Enables NtQueryEvent.
// http://undocumented.ntinternals.net/index.html?page=UserMode%2FUndocumented%20Functions%2FNT%20Objects%2FEvent%2FNtQueryEvent.html
const DWORD_ EVENT_QUERY_STATE_ = 0x00000001;

const DWORD_ event_all_access = EVENT_ALL_ACCESS_;
const DWORD_ event_modify_state = EVENT_MODIFY_STATE_;
#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
const DWORD_ create_event_initial_set = CREATE_EVENT_INITIAL_SET_;
const DWORD_ create_event_manual_reset = CREATE_EVENT_MANUAL_RESET_;
#endif

#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_FORCEINLINE HANDLE_ CreateEventA(SECURITY_ATTRIBUTES_* lpEventAttributes, BOOL_ bManualReset, BOOL_ bInitialState, LPCSTR_ lpName)
{
#if AUTOBOOST_PLAT_WINDOWS_RUNTIME && AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
    const DWORD_ flags = (bManualReset ? create_event_manual_reset : 0u) | (bInitialState ? create_event_initial_set : 0u);
    return ::CreateEventExA(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpEventAttributes), lpName, flags, event_all_access);
#else
    return ::CreateEventA(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpEventAttributes), bManualReset, bInitialState, lpName);
#endif
}

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
AUTOBOOST_FORCEINLINE HANDLE_ CreateEventExA(SECURITY_ATTRIBUTES_* lpEventAttributes, LPCSTR_ lpName, DWORD_ dwFlags, DWORD_ dwDesiredAccess)
{
    return ::CreateEventExA(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpEventAttributes), lpName, dwFlags, dwDesiredAccess);
}
#endif
#endif

AUTOBOOST_FORCEINLINE HANDLE_ CreateEventW(SECURITY_ATTRIBUTES_* lpEventAttributes, BOOL_ bManualReset, BOOL_ bInitialState, LPCWSTR_ lpName)
{
#if AUTOBOOST_PLAT_WINDOWS_RUNTIME && AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
    const DWORD_ flags = (bManualReset ? create_event_manual_reset : 0u) | (bInitialState ? create_event_initial_set : 0u);
    return ::CreateEventExW(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpEventAttributes), lpName, flags, event_all_access);
#else
    return ::CreateEventW(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpEventAttributes), bManualReset, bInitialState, lpName);
#endif
}

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
AUTOBOOST_FORCEINLINE HANDLE_ CreateEventExW(SECURITY_ATTRIBUTES_* lpEventAttributes, LPCWSTR_ lpName, DWORD_ dwFlags, DWORD_ dwDesiredAccess)
{
    return ::CreateEventExW(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpEventAttributes), lpName, dwFlags, dwDesiredAccess);
}
#endif

#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_FORCEINLINE HANDLE_ create_event(SECURITY_ATTRIBUTES_* lpEventAttributes, BOOL_ bManualReset, BOOL_ bInitialState, LPCSTR_ lpName)
{
    return winapi::CreateEventA(lpEventAttributes, bManualReset, bInitialState, lpName);
}

AUTOBOOST_FORCEINLINE HANDLE_ open_event(DWORD_ dwDesiredAccess, BOOL_ bInheritHandle, LPCSTR_ lpName)
{
    return ::OpenEventA(dwDesiredAccess, bInheritHandle, lpName);
}
#endif

AUTOBOOST_FORCEINLINE HANDLE_ create_event(SECURITY_ATTRIBUTES_* lpEventAttributes, BOOL_ bManualReset, BOOL_ bInitialState, LPCWSTR_ lpName)
{
    return winapi::CreateEventW(lpEventAttributes, bManualReset, bInitialState, lpName);
}

AUTOBOOST_FORCEINLINE HANDLE_ open_event(DWORD_ dwDesiredAccess, BOOL_ bInheritHandle, LPCWSTR_ lpName)
{
    return ::OpenEventW(dwDesiredAccess, bInheritHandle, lpName);
}

AUTOBOOST_FORCEINLINE HANDLE_ create_anonymous_event(SECURITY_ATTRIBUTES_* lpEventAttributes, BOOL_ bManualReset, BOOL_ bInitialState)
{
    return winapi::CreateEventW(lpEventAttributes, bManualReset, bInitialState, 0);
}

}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_EVENT_HPP
