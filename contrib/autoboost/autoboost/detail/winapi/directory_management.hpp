//  directory_management.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_DIRECTORY_MANAGEMENT_HPP
#define AUTOBOOST_DETAIL_WINAPI_DIRECTORY_MANAGEMENT_HPP

#include <autoboost/detail/winapi/basic_types.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
    CreateDirectoryA(autoboost::detail::winapi::LPCSTR_, ::_SECURITY_ATTRIBUTES*);
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
    GetTempPathA(autoboost::detail::winapi::DWORD_ length, autoboost::detail::winapi::LPSTR_ buffer);
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
    RemoveDirectoryA(autoboost::detail::winapi::LPCSTR_);
#endif
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
    CreateDirectoryW(autoboost::detail::winapi::LPCWSTR_, ::_SECURITY_ATTRIBUTES*);
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
    GetTempPathW(autoboost::detail::winapi::DWORD_ length, autoboost::detail::winapi::LPWSTR_ buffer);
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
    RemoveDirectoryW(autoboost::detail::winapi::LPCWSTR_);
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {

#if !defined( AUTOBOOST_NO_ANSI_APIS )
using ::GetTempPathA;
using ::RemoveDirectoryA;
#endif
using ::GetTempPathW;
using ::RemoveDirectoryW;

#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_FORCEINLINE BOOL_ CreateDirectoryA(LPCSTR_ pPathName, PSECURITY_ATTRIBUTES_ pSecurityAttributes)
{
    return ::CreateDirectoryA(pPathName, reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(pSecurityAttributes));
}
#endif

AUTOBOOST_FORCEINLINE BOOL_ CreateDirectoryW(LPCWSTR_ pPathName, PSECURITY_ATTRIBUTES_ pSecurityAttributes)
{
    return ::CreateDirectoryW(pPathName, reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(pSecurityAttributes));
}

#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_FORCEINLINE BOOL_ create_directory(LPCSTR_ pPathName, PSECURITY_ATTRIBUTES_ pSecurityAttributes)
{
    return ::CreateDirectoryA(pPathName, reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(pSecurityAttributes));
}
AUTOBOOST_FORCEINLINE DWORD_ get_temp_path(DWORD_ length, LPSTR_ buffer)
{
    return ::GetTempPathA(length, buffer);
}
AUTOBOOST_FORCEINLINE BOOL_ remove_directory(LPCSTR_ pPathName)
{
    return ::RemoveDirectoryA(pPathName);
}
#endif

AUTOBOOST_FORCEINLINE BOOL_ create_directory(LPCWSTR_ pPathName, PSECURITY_ATTRIBUTES_ pSecurityAttributes)
{
    return ::CreateDirectoryW(pPathName, reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(pSecurityAttributes));
}
AUTOBOOST_FORCEINLINE DWORD_ get_temp_path(DWORD_ length, LPWSTR_ buffer)
{
    return ::GetTempPathW(length, buffer);
}
AUTOBOOST_FORCEINLINE BOOL_ remove_directory(LPCWSTR_ pPathName)
{
    return ::RemoveDirectoryW(pPathName);
}

} // namespace winapi
} // namespace detail
} // namespace autoboost

#endif // AUTOBOOST_DETAIL_WINAPI_DIRECTORY_MANAGEMENT_HPP
