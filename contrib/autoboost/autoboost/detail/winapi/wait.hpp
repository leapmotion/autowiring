//  wait.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_WAIT_HPP
#define AUTOBOOST_DETAIL_WINAPI_WAIT_HPP

#include <autoboost/detail/winapi/basic_types.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
WaitForSingleObject(
    autoboost::detail::winapi::HANDLE_ hHandle,
    autoboost::detail::winapi::DWORD_ dwMilliseconds);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
WaitForMultipleObjects(
    autoboost::detail::winapi::DWORD_ nCount,
    autoboost::detail::winapi::HANDLE_ const* lpHandles,
    autoboost::detail::winapi::BOOL_ bWaitAll,
    autoboost::detail::winapi::DWORD_ dwMilliseconds);

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_NT4
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
SignalObjectAndWait(
    autoboost::detail::winapi::HANDLE_ hObjectToSignal,
    autoboost::detail::winapi::HANDLE_ hObjectToWaitOn,
    autoboost::detail::winapi::DWORD_ dwMilliseconds,
    autoboost::detail::winapi::BOOL_ bAlertable);
#endif
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {

using ::WaitForMultipleObjects;
using ::WaitForSingleObject;
#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_NT4
using ::SignalObjectAndWait;
#endif

#if defined( AUTOBOOST_USE_WINDOWS_H )

const DWORD_ INFINITE_ = INFINITE;
const DWORD_ WAIT_ABANDONED_ = WAIT_ABANDONED;
const DWORD_ WAIT_OBJECT_0_ = WAIT_OBJECT_0;
const DWORD_ WAIT_TIMEOUT_ = WAIT_TIMEOUT;
const DWORD_ WAIT_FAILED_ = WAIT_FAILED;

#else // defined( AUTOBOOST_USE_WINDOWS_H )

const DWORD_ INFINITE_ = (DWORD_)0xFFFFFFFF;
const DWORD_ WAIT_ABANDONED_ = 0x00000080L;
const DWORD_ WAIT_OBJECT_0_ = 0x00000000L;
const DWORD_ WAIT_TIMEOUT_ = 0x00000102L;
const DWORD_ WAIT_FAILED_ = (DWORD_)0xFFFFFFFF;

#endif // defined( AUTOBOOST_USE_WINDOWS_H )

const DWORD_ infinite = INFINITE_;
const DWORD_ wait_abandoned = WAIT_ABANDONED_;
const DWORD_ wait_object_0 = WAIT_OBJECT_0_;
const DWORD_ wait_timeout = WAIT_TIMEOUT_;
const DWORD_ wait_failed = WAIT_FAILED_;

const DWORD_ max_non_infinite_wait = (DWORD_)0xFFFFFFFE;

}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_WAIT_HPP
