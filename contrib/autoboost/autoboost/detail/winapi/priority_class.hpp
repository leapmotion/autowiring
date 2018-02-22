//  priority_class.hpp  --------------------------------------------------------------//

//  Copyright 2016 Klemens D. Morgenstern
//  Copyright 2016 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#ifndef AUTOBOOST_DETAIL_WINAPI_PRIORITY_CLASS_HPP_
#define AUTOBOOST_DETAIL_WINAPI_PRIORITY_CLASS_HPP_

#include <autoboost/detail/winapi/basic_types.hpp>
#include <autoboost/predef/platform.h>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if AUTOBOOST_PLAT_WINDOWS_DESKTOP

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
GetPriorityClass(autoboost::detail::winapi::HANDLE_ hProcess);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
SetPriorityClass(
    autoboost::detail::winapi::HANDLE_ hProcess,
    autoboost::detail::winapi::DWORD_ dwPriorityClass);

} // extern "C"
#endif //defined AUTOBOOST_WINDOWS_H

namespace autoboost {
namespace detail {
namespace winapi {

#if defined(AUTOBOOST_USE_WINDOWS_H)

const DWORD_ NORMAL_PRIORITY_CLASS_            = NORMAL_PRIORITY_CLASS;
const DWORD_ IDLE_PRIORITY_CLASS_              = IDLE_PRIORITY_CLASS;
const DWORD_ HIGH_PRIORITY_CLASS_              = HIGH_PRIORITY_CLASS;
const DWORD_ REALTIME_PRIORITY_CLASS_          = REALTIME_PRIORITY_CLASS;
const DWORD_ BELOW_NORMAL_PRIORITY_CLASS_      = BELOW_NORMAL_PRIORITY_CLASS;
const DWORD_ ABOVE_NORMAL_PRIORITY_CLASS_      = ABOVE_NORMAL_PRIORITY_CLASS;

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
const DWORD_ PROCESS_MODE_BACKGROUND_BEGIN_    = PROCESS_MODE_BACKGROUND_BEGIN;
const DWORD_ PROCESS_MODE_BACKGROUND_END_      = PROCESS_MODE_BACKGROUND_END;
#endif

#else // defined( AUTOBOOST_USE_WINDOWS_H )

const DWORD_ NORMAL_PRIORITY_CLASS_            = 0x20;
const DWORD_ IDLE_PRIORITY_CLASS_              = 0x40;
const DWORD_ HIGH_PRIORITY_CLASS_              = 0x80;
const DWORD_ REALTIME_PRIORITY_CLASS_          = 0x100;
const DWORD_ BELOW_NORMAL_PRIORITY_CLASS_      = 0x4000;
const DWORD_ ABOVE_NORMAL_PRIORITY_CLASS_      = 0x8000;

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
const DWORD_ PROCESS_MODE_BACKGROUND_BEGIN_    = 0x100000;
const DWORD_ PROCESS_MODE_BACKGROUND_END_      = 0x200000;
#endif

#endif // defined( AUTOBOOST_USE_WINDOWS_H )

using ::GetPriorityClass;
using ::SetPriorityClass;

}
}
}

#endif // AUTOBOOST_PLAT_WINDOWS_DESKTOP

#endif // AUTOBOOST_DETAIL_WINAPI_PRIORITY_CLASS_HPP_
