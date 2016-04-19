//  thread.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_THREAD_HPP
#define AUTOBOOST_DETAIL_WINAPI_THREAD_HPP

#include <autoboost/detail/winapi/basic_types.hpp>
#include <autoboost/detail/winapi/GetCurrentThread.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
// Windows CE define GetCurrentThreadId as an inline function in kfuncs.h
#if !defined( UNDER_CE )
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI GetCurrentThreadId(AUTOBOOST_DETAIL_WINAPI_VOID);
#endif
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
SleepEx(
    autoboost::detail::winapi::DWORD_ dwMilliseconds,
    autoboost::detail::winapi::BOOL_ bAlertable);
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI Sleep(autoboost::detail::winapi::DWORD_ dwMilliseconds);
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI SwitchToThread(AUTOBOOST_DETAIL_WINAPI_VOID);
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {
using ::GetCurrentThreadId;
using ::SleepEx;
using ::Sleep;
using ::SwitchToThread;
}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_THREAD_HPP
