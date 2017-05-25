//  get_system_directory.hpp  --------------------------------------------------------------//

//  Copyright 2016 Klemens D. Morgenstern

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#ifndef AUTOBOOST_DETAIL_WINAPI_GET_SYSTEM_DIRECTORY_HPP_
#define AUTOBOOST_DETAIL_WINAPI_GET_SYSTEM_DIRECTORY_HPP_

#include <autoboost/detail/winapi/basic_types.hpp>
#include <autoboost/predef/platform.h>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if AUTOBOOST_PLAT_WINDOWS_DESKTOP

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::UINT_ WINAPI
GetSystemDirectoryA(
    autoboost::detail::winapi::LPSTR_ lpBuffer,
    autoboost::detail::winapi::UINT_   uSize);
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::UINT_ WINAPI
GetSystemDirectoryW(
    autoboost::detail::winapi::LPWSTR_ lpBuffer,
    autoboost::detail::winapi::UINT_   uSize);
} // extern "C"
#endif

namespace autoboost {
namespace detail {
namespace winapi {

#if !defined( AUTOBOOST_NO_ANSI_APIS )
using ::GetSystemDirectoryA;
#endif
using ::GetSystemDirectoryW;

#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_FORCEINLINE UINT_ get_system_directory(LPSTR_ lpBuffer,  UINT_ uSize)
{
    return ::GetSystemDirectoryA(lpBuffer, uSize);
}
#endif

AUTOBOOST_FORCEINLINE UINT_ get_system_directory(LPWSTR_ lpBuffer,  UINT_ uSize)
{
    return ::GetSystemDirectoryW(lpBuffer, uSize);
}

}
}
}

#endif // AUTOBOOST_PLAT_WINDOWS_DESKTOP

#endif // AUTOBOOST_DETAIL_WINAPI_GET_SYSTEM_DIRECTORY_HPP_
