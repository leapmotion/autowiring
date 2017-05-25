//  handle_info.hpp  --------------------------------------------------------------//

//  Copyright 2016 Klemens D. Morgenstern

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#ifndef AUTOBOOST_DETAIL_HANDLE_INFO_HPP_
#define AUTOBOOST_DETAIL_HANDLE_INFO_HPP_

#include <autoboost/detail/winapi/basic_types.hpp>
#include <autoboost/predef/platform.h>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if AUTOBOOST_PLAT_WINDOWS_DESKTOP

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
GetHandleInformation(
        autoboost::detail::winapi::HANDLE_ hObject,
        autoboost::detail::winapi::LPDWORD_ lpdwFlags);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
SetHandleInformation(
        autoboost::detail::winapi::HANDLE_ hObject,
        autoboost::detail::winapi::DWORD_ dwMask,
        autoboost::detail::winapi::DWORD_ dwFlags);

} // extern "C"
#endif

namespace autoboost {
namespace detail {
namespace winapi {

using ::GetHandleInformation;
using ::SetHandleInformation;

#if defined( AUTOBOOST_USE_WINDOWS_H )

const DWORD_ HANDLE_FLAG_INHERIT_            = HANDLE_FLAG_INHERIT;
const DWORD_ HANDLE_FLAG_PROTECT_FROM_CLOSE_ = HANDLE_FLAG_PROTECT_FROM_CLOSE;

#else

const DWORD_ HANDLE_FLAG_INHERIT_            = 0x1;
const DWORD_ HANDLE_FLAG_PROTECT_FROM_CLOSE_ = 0x2;

#endif

}
}
}

#endif // AUTOBOOST_PLAT_WINDOWS_DESKTOP

#endif // AUTOBOOST_DETAIL_HANDLE_INFO_HPP_
