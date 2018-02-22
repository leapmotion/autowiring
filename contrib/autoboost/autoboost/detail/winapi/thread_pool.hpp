//  thread_pool.hpp  --------------------------------------------------------------//

//  Copyright 2013 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_THREAD_POOL_HPP
#define AUTOBOOST_DETAIL_WINAPI_THREAD_POOL_HPP

#include <autoboost/detail/winapi/config.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN2K

#include <autoboost/detail/winapi/basic_types.hpp>

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
typedef autoboost::detail::winapi::VOID_ (NTAPI *WAITORTIMERCALLBACKFUNC)
(autoboost::detail::winapi::PVOID_, autoboost::detail::winapi::BOOLEAN_);
typedef WAITORTIMERCALLBACKFUNC WAITORTIMERCALLBACK;

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
RegisterWaitForSingleObject(
    autoboost::detail::winapi::PHANDLE_ phNewWaitObject,
    autoboost::detail::winapi::HANDLE_ hObject,
    WAITORTIMERCALLBACK Callback,
    autoboost::detail::winapi::PVOID_ Context,
    autoboost::detail::winapi::ULONG_ dwMilliseconds,
    autoboost::detail::winapi::ULONG_ dwFlags);
}
#endif

// MinGW is buggy - it is missing these function declarations for Win2000
#if !defined( AUTOBOOST_USE_WINDOWS_H ) || (defined(AUTOBOOST_WINAPI_IS_MINGW) && AUTOBOOST_USE_WINAPI_VERSION < AUTOBOOST_WINAPI_VERSION_WINXP)
extern "C" {
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
UnregisterWait(autoboost::detail::winapi::HANDLE_ WaitHandle);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
UnregisterWaitEx(
    autoboost::detail::winapi::HANDLE_ WaitHandle,
    autoboost::detail::winapi::HANDLE_ CompletionEvent);
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {

typedef ::WAITORTIMERCALLBACKFUNC WAITORTIMERCALLBACKFUNC_;
typedef ::WAITORTIMERCALLBACK WAITORTIMERCALLBACK_;

using ::RegisterWaitForSingleObject;
using ::UnregisterWait;
using ::UnregisterWaitEx;

#if defined( AUTOBOOST_USE_WINDOWS_H )

const ULONG_ WT_EXECUTEDEFAULT_ = WT_EXECUTEDEFAULT;
const ULONG_ WT_EXECUTEINIOTHREAD_ = WT_EXECUTEINIOTHREAD;
#if defined( AUTOBOOST_WINAPI_IS_MINGW )
const ULONG_ WT_EXECUTEINUITHREAD_ = 0x00000002;
#else
const ULONG_ WT_EXECUTEINUITHREAD_ = WT_EXECUTEINUITHREAD;
#endif
const ULONG_ WT_EXECUTEINWAITTHREAD_ = WT_EXECUTEINWAITTHREAD;
const ULONG_ WT_EXECUTEONLYONCE_ = WT_EXECUTEONLYONCE;
const ULONG_ WT_EXECUTEINTIMERTHREAD_ = WT_EXECUTEINTIMERTHREAD;
const ULONG_ WT_EXECUTELONGFUNCTION_ = WT_EXECUTELONGFUNCTION;
#if defined( AUTOBOOST_WINAPI_IS_MINGW )
const ULONG_ WT_EXECUTEINPERSISTENTIOTHREAD_ = 0x00000040;
#else
const ULONG_ WT_EXECUTEINPERSISTENTIOTHREAD_ = WT_EXECUTEINPERSISTENTIOTHREAD;
#endif
const ULONG_ WT_EXECUTEINPERSISTENTTHREAD_ = WT_EXECUTEINPERSISTENTTHREAD;
const ULONG_ WT_TRANSFER_IMPERSONATION_ = WT_TRANSFER_IMPERSONATION;

inline ULONG_ wt_set_max_threadpool_threads(ULONG_ flags, ULONG_ limit)
{
    return WT_SET_MAX_THREADPOOL_THREADS(flags, limit);
}

#else // defined( AUTOBOOST_USE_WINDOWS_H )

const ULONG_ WT_EXECUTEDEFAULT_ = 0x00000000;
const ULONG_ WT_EXECUTEINIOTHREAD_ = 0x00000001;
const ULONG_ WT_EXECUTEINUITHREAD_ = 0x00000002;
const ULONG_ WT_EXECUTEINWAITTHREAD_ = 0x00000004;
const ULONG_ WT_EXECUTEONLYONCE_ = 0x00000008;
const ULONG_ WT_EXECUTEINTIMERTHREAD_ = 0x00000020;
const ULONG_ WT_EXECUTELONGFUNCTION_ = 0x00000010;
const ULONG_ WT_EXECUTEINPERSISTENTIOTHREAD_ = 0x00000040;
const ULONG_ WT_EXECUTEINPERSISTENTTHREAD_ = 0x00000080;
const ULONG_ WT_TRANSFER_IMPERSONATION_ = 0x00000100;

inline ULONG_ wt_set_max_threadpool_threads(ULONG_ flags, ULONG_ limit)
{
    return flags | (limit << 16);
}

#endif // defined( AUTOBOOST_USE_WINDOWS_H )

const ULONG_ wt_execute_default = WT_EXECUTEDEFAULT_;
const ULONG_ wt_execute_in_io_thread = WT_EXECUTEINIOTHREAD_;
const ULONG_ wt_execute_in_ui_thread = WT_EXECUTEINUITHREAD_;
const ULONG_ wt_execute_in_wait_thread = WT_EXECUTEINWAITTHREAD_;
const ULONG_ wt_execute_only_once = WT_EXECUTEONLYONCE_;
const ULONG_ wt_execute_in_timer_thread = WT_EXECUTEINTIMERTHREAD_;
const ULONG_ wt_execute_long_function = WT_EXECUTELONGFUNCTION_;
const ULONG_ wt_execute_in_persistent_io_thread = WT_EXECUTEINPERSISTENTIOTHREAD_;
const ULONG_ wt_execute_in_persistent_thread = WT_EXECUTEINPERSISTENTTHREAD_;
const ULONG_ wt_transfer_impersonation = WT_TRANSFER_IMPERSONATION_;

}
}
}

#endif // AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN2K

#endif // AUTOBOOST_DETAIL_WINAPI_THREAD_POOL_HPP
