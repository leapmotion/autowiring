//  mutex.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_MUTEX_HPP
#define AUTOBOOST_DETAIL_WINAPI_MUTEX_HPP

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
CreateMutexA(
    ::_SECURITY_ATTRIBUTES* lpMutexAttributes,
    autoboost::detail::winapi::BOOL_ bInitialOwner,
    autoboost::detail::winapi::LPCSTR_ lpName);
#endif

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
CreateMutexExA(
    ::_SECURITY_ATTRIBUTES* lpMutexAttributes,
    autoboost::detail::winapi::LPCSTR_ lpName,
    autoboost::detail::winapi::DWORD_ dwFlags,
    autoboost::detail::winapi::DWORD_ dwDesiredAccess);
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
OpenMutexA(
    autoboost::detail::winapi::DWORD_ dwDesiredAccess,
    autoboost::detail::winapi::BOOL_ bInheritHandle,
    autoboost::detail::winapi::LPCSTR_ lpName);
#endif

#if !defined( AUTOBOOST_PLAT_WINDOWS_RUNTIME_AVALIABLE )
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
CreateMutexW(
    ::_SECURITY_ATTRIBUTES* lpMutexAttributes,
    autoboost::detail::winapi::BOOL_ bInitialOwner,
    autoboost::detail::winapi::LPCWSTR_ lpName);
#endif

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
CreateMutexExW(
    ::_SECURITY_ATTRIBUTES* lpMutexAttributes,
    autoboost::detail::winapi::LPCWSTR_ lpName,
    autoboost::detail::winapi::DWORD_ dwFlags,
    autoboost::detail::winapi::DWORD_ dwDesiredAccess);
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
OpenMutexW(
    autoboost::detail::winapi::DWORD_ dwDesiredAccess,
    autoboost::detail::winapi::BOOL_ bInheritHandle,
    autoboost::detail::winapi::LPCWSTR_ lpName);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
ReleaseMutex(autoboost::detail::winapi::HANDLE_ hMutex);
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {

#if !defined( AUTOBOOST_NO_ANSI_APIS )
using ::OpenMutexA;
#endif
using ::OpenMutexW;
using ::ReleaseMutex;

#if defined( AUTOBOOST_USE_WINDOWS_H )

const DWORD_ MUTEX_ALL_ACCESS_ = MUTEX_ALL_ACCESS;
#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
const DWORD_ CREATE_MUTEX_INITIAL_OWNER_ = CREATE_MUTEX_INITIAL_OWNER;
#endif

#else // defined( AUTOBOOST_USE_WINDOWS_H )

const DWORD_ MUTEX_ALL_ACCESS_ = 0x1F0001;
#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
const DWORD_ CREATE_MUTEX_INITIAL_OWNER_ = 0x00000001;
#endif

#endif // defined( AUTOBOOST_USE_WINDOWS_H )

const DWORD_ mutex_all_access = MUTEX_ALL_ACCESS_;
#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
const DWORD_ create_mutex_initial_owner = CREATE_MUTEX_INITIAL_OWNER_;
#endif

#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_FORCEINLINE HANDLE_ CreateMutexA(SECURITY_ATTRIBUTES_* lpMutexAttributes, BOOL_ bInitialOwner, LPCSTR_ lpName)
{
#if AUTOBOOST_PLAT_WINDOWS_RUNTIME && AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
    const DWORD_ flags = bInitialOwner ? create_mutex_initial_owner : 0u;
    return ::CreateMutexExA(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpMutexAttributes), lpName, flags, mutex_all_access);
#else
    return ::CreateMutexA(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpMutexAttributes), bInitialOwner, lpName);
#endif
}

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
AUTOBOOST_FORCEINLINE HANDLE_ CreateMutexExA(
    SECURITY_ATTRIBUTES_* lpMutexAttributes,
    LPCSTR_ lpName,
    DWORD_ dwFlags,
    DWORD_ dwDesiredAccess)
{
    return ::CreateMutexExA(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpMutexAttributes), lpName, dwFlags, dwDesiredAccess);
}
#endif
#endif

AUTOBOOST_FORCEINLINE HANDLE_ CreateMutexW(SECURITY_ATTRIBUTES_* lpMutexAttributes, BOOL_ bInitialOwner, LPCWSTR_ lpName)
{
#if AUTOBOOST_PLAT_WINDOWS_RUNTIME && AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
    const DWORD_ flags = bInitialOwner ? create_mutex_initial_owner : 0u;
    return ::CreateMutexExW(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpMutexAttributes), lpName, flags, mutex_all_access);
#else
    return ::CreateMutexW(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpMutexAttributes), bInitialOwner, lpName);
#endif
}

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
AUTOBOOST_FORCEINLINE HANDLE_ CreateMutexExW(
    SECURITY_ATTRIBUTES_* lpMutexAttributes,
    LPCWSTR_ lpName,
    DWORD_ dwFlags,
    DWORD_ dwDesiredAccess)
{
    return ::CreateMutexExW(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpMutexAttributes), lpName, dwFlags, dwDesiredAccess);
}
#endif

#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_FORCEINLINE HANDLE_ create_mutex(SECURITY_ATTRIBUTES_* lpAttributes, BOOL_ bInitialOwner, LPCSTR_ lpName)
{
    return winapi::CreateMutexA(lpAttributes, bInitialOwner, lpName);
}

AUTOBOOST_FORCEINLINE HANDLE_ open_mutex(DWORD_ dwDesiredAccess, BOOL_ bInheritHandle, LPCSTR_ lpName)
{
    return ::OpenMutexA(dwDesiredAccess, bInheritHandle, lpName);
}
#endif

AUTOBOOST_FORCEINLINE HANDLE_ create_mutex(SECURITY_ATTRIBUTES_* lpAttributes, BOOL_ bInitialOwner, LPCWSTR_ lpName)
{
    return winapi::CreateMutexW(lpAttributes, bInitialOwner, lpName);
}

AUTOBOOST_FORCEINLINE HANDLE_ open_mutex(DWORD_ dwDesiredAccess, BOOL_ bInheritHandle, LPCWSTR_ lpName)
{
    return ::OpenMutexW(dwDesiredAccess, bInheritHandle, lpName);
}

AUTOBOOST_FORCEINLINE HANDLE_ create_anonymous_mutex(SECURITY_ATTRIBUTES_* lpAttributes, BOOL_ bInitialOwner)
{
    return winapi::CreateMutexW(lpAttributes, bInitialOwner, 0);
}

}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_MUTEX_HPP
