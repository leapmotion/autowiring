//  environment.hpp  --------------------------------------------------------------//

//  Copyright 2016 Klemens D. Morgenstern

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#ifndef AUTOBOOST_DETAIL_WINAPI_ENVIRONMENT_HPP_
#define AUTOBOOST_DETAIL_WINAPI_ENVIRONMENT_HPP_

#include <autoboost/detail/winapi/basic_types.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::LPSTR_ WINAPI GetEnvironmentStringsA();
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI FreeEnvironmentStringsA(autoboost::detail::winapi::LPSTR_);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI GetEnvironmentVariableA(
    autoboost::detail::winapi::LPCSTR_ lpName,
    autoboost::detail::winapi::LPSTR_ lpBuffer,
    autoboost::detail::winapi::DWORD_ nSize
);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI SetEnvironmentVariableA(
    autoboost::detail::winapi::LPCSTR_ lpName,
    autoboost::detail::winapi::LPCSTR_ lpValue
);
#endif // !defined( AUTOBOOST_NO_ANSI_APIS )

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::LPWSTR_ WINAPI GetEnvironmentStringsW();
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI FreeEnvironmentStringsW(autoboost::detail::winapi::LPWSTR_);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI GetEnvironmentVariableW(
    autoboost::detail::winapi::LPCWSTR_ lpName,
    autoboost::detail::winapi::LPWSTR_ lpBuffer,
    autoboost::detail::winapi::DWORD_ nSize
);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI SetEnvironmentVariableW(
    autoboost::detail::winapi::LPCWSTR_ lpName,
    autoboost::detail::winapi::LPCWSTR_ lpValue
);
} // extern "C"
#endif // !defined( AUTOBOOST_USE_WINDOWS_H )

namespace autoboost { namespace detail { namespace winapi {

#if !defined( AUTOBOOST_NO_ANSI_APIS )
using ::GetEnvironmentStringsA;
using ::FreeEnvironmentStringsA;
using ::GetEnvironmentVariableA;
using ::SetEnvironmentVariableA;
#endif // !defined( AUTOBOOST_NO_ANSI_APIS )

using ::GetEnvironmentStringsW;
using ::FreeEnvironmentStringsW;
using ::GetEnvironmentVariableW;
using ::SetEnvironmentVariableW;

template< typename Char >
Char* get_environment_strings();

#if !defined( AUTOBOOST_NO_ANSI_APIS )

template< >
AUTOBOOST_FORCEINLINE char* get_environment_strings< char >()
{
    return GetEnvironmentStringsA();
}

AUTOBOOST_FORCEINLINE BOOL_ free_environment_strings(autoboost::detail::winapi::LPSTR_ p)
{
    return FreeEnvironmentStringsA(p);
}

AUTOBOOST_FORCEINLINE DWORD_ get_environment_variable(LPCSTR_ name, LPSTR_ buffer, DWORD_ size)
{
    return GetEnvironmentVariableA(name, buffer, size);
}

AUTOBOOST_FORCEINLINE BOOL_ set_environment_variable(LPCSTR_ name, LPCSTR_ value)
{
    return SetEnvironmentVariableA(name, value);
}

#endif // !defined( AUTOBOOST_NO_ANSI_APIS )

template< >
AUTOBOOST_FORCEINLINE wchar_t* get_environment_strings< wchar_t >()
{
    return GetEnvironmentStringsW();
}

AUTOBOOST_FORCEINLINE BOOL_ free_environment_strings(autoboost::detail::winapi::LPWSTR_ p)
{
    return FreeEnvironmentStringsW(p);
}

AUTOBOOST_FORCEINLINE DWORD_ get_environment_variable(LPCWSTR_ name, LPWSTR_ buffer, DWORD_ size)
{
    return GetEnvironmentVariableW(name, buffer, size);
}

AUTOBOOST_FORCEINLINE BOOL_ set_environment_variable(LPCWSTR_ name, LPCWSTR_ value)
{
    return SetEnvironmentVariableW(name, value);
}

} // namespace winapi
} // namespace detail
} // namespace autoboost

#endif // AUTOBOOST_DETAIL_WINAPI_ENVIRONMENT_HPP_
