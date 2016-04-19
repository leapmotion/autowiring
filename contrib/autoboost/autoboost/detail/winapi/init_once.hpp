//  init_once.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_INIT_ONCE_HPP
#define AUTOBOOST_DETAIL_WINAPI_INIT_ONCE_HPP

#include <autoboost/detail/winapi/config.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6

#include <autoboost/detail/winapi/basic_types.hpp>

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
#if defined( AUTOBOOST_WINAPI_IS_MINGW_W64 )
struct _RTL_RUN_ONCE;
#else
union _RTL_RUN_ONCE;
#endif

typedef autoboost::detail::winapi::BOOL_
(WINAPI *PINIT_ONCE_FN) (
    ::_RTL_RUN_ONCE* InitOnce,
    autoboost::detail::winapi::PVOID_ Parameter,
    autoboost::detail::winapi::PVOID_ *Context);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
InitOnceInitialize(::_RTL_RUN_ONCE* InitOnce);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
InitOnceExecuteOnce(
    ::_RTL_RUN_ONCE* InitOnce,
    ::PINIT_ONCE_FN InitFn,
    autoboost::detail::winapi::PVOID_ Parameter,
    autoboost::detail::winapi::LPVOID_ *Context);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
InitOnceBeginInitialize(
    ::_RTL_RUN_ONCE* lpInitOnce,
    autoboost::detail::winapi::DWORD_ dwFlags,
    autoboost::detail::winapi::PBOOL_ fPending,
    autoboost::detail::winapi::LPVOID_ *lpContext);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
InitOnceComplete(
    ::_RTL_RUN_ONCE* lpInitOnce,
    autoboost::detail::winapi::DWORD_ dwFlags,
    autoboost::detail::winapi::LPVOID_ lpContext);
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {

typedef union AUTOBOOST_DETAIL_WINAPI_MAY_ALIAS _RTL_RUN_ONCE {
    PVOID_ Ptr;
} INIT_ONCE_, *PINIT_ONCE_, *LPINIT_ONCE_;

extern "C" {
typedef BOOL_ (WINAPI *PINIT_ONCE_FN_) (PINIT_ONCE_ lpInitOnce, PVOID_ Parameter, PVOID_ *Context);
}

AUTOBOOST_FORCEINLINE VOID_ InitOnceInitialize(PINIT_ONCE_ lpInitOnce)
{
    ::InitOnceInitialize(reinterpret_cast< ::_RTL_RUN_ONCE* >(lpInitOnce));
}

AUTOBOOST_FORCEINLINE BOOL_ InitOnceExecuteOnce(PINIT_ONCE_ lpInitOnce, PINIT_ONCE_FN_ InitFn, PVOID_ Parameter, LPVOID_ *Context)
{
    return ::InitOnceExecuteOnce(reinterpret_cast< ::_RTL_RUN_ONCE* >(lpInitOnce), reinterpret_cast< ::PINIT_ONCE_FN >(InitFn), Parameter, Context);
}

AUTOBOOST_FORCEINLINE BOOL_ InitOnceBeginInitialize(PINIT_ONCE_ lpInitOnce, DWORD_ dwFlags, PBOOL_ fPending, LPVOID_ *lpContext)
{
    return ::InitOnceBeginInitialize(reinterpret_cast< ::_RTL_RUN_ONCE* >(lpInitOnce), dwFlags, fPending, lpContext);
}

AUTOBOOST_FORCEINLINE BOOL_ InitOnceComplete(PINIT_ONCE_ lpInitOnce, DWORD_ dwFlags, LPVOID_ lpContext)
{
    return ::InitOnceComplete(reinterpret_cast< ::_RTL_RUN_ONCE* >(lpInitOnce), dwFlags, lpContext);
}

#if defined( AUTOBOOST_USE_WINDOWS_H )

#define AUTOBOOST_DETAIL_WINAPI_INIT_ONCE_STATIC_INIT INIT_ONCE_STATIC_INIT
const DWORD_ INIT_ONCE_ASYNC_ = INIT_ONCE_ASYNC;
const DWORD_ INIT_ONCE_CHECK_ONLY_ = INIT_ONCE_CHECK_ONLY;
const DWORD_ INIT_ONCE_INIT_FAILED_ = INIT_ONCE_INIT_FAILED;
const DWORD_ INIT_ONCE_CTX_RESERVED_BITS_ = INIT_ONCE_CTX_RESERVED_BITS;

#else // defined( AUTOBOOST_USE_WINDOWS_H )

#define AUTOBOOST_DETAIL_WINAPI_INIT_ONCE_STATIC_INIT {0}
const DWORD_ INIT_ONCE_ASYNC_ = 0x00000002UL;
const DWORD_ INIT_ONCE_CHECK_ONLY_ = 0x00000001UL;
const DWORD_ INIT_ONCE_INIT_FAILED_ = 0x00000004UL;
const DWORD_ INIT_ONCE_CTX_RESERVED_BITS_ = 2;

#endif // defined( AUTOBOOST_USE_WINDOWS_H )

const DWORD_ init_once_async = INIT_ONCE_ASYNC_;
const DWORD_ init_once_check_only = INIT_ONCE_CHECK_ONLY_;
const DWORD_ init_once_init_failed = INIT_ONCE_INIT_FAILED_;
const DWORD_ init_once_ctx_reserved_bits = INIT_ONCE_CTX_RESERVED_BITS_;

}
}
}

#endif // AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6

#endif // AUTOBOOST_DETAIL_WINAPI_INIT_ONCE_HPP
