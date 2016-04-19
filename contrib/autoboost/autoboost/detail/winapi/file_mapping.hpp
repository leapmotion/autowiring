//  file_mapping.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_FILE_MAPPING_HPP
#define AUTOBOOST_DETAIL_WINAPI_FILE_MAPPING_HPP

#include <autoboost/detail/winapi/basic_types.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
CreateFileMappingA(
    autoboost::detail::winapi::HANDLE_ hFile,
    ::_SECURITY_ATTRIBUTES* lpFileMappingAttributes,
    autoboost::detail::winapi::DWORD_ flProtect,
    autoboost::detail::winapi::DWORD_ dwMaximumSizeHigh,
    autoboost::detail::winapi::DWORD_ dwMaximumSizeLow,
    autoboost::detail::winapi::LPCSTR_ lpName);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
OpenFileMappingA(
    autoboost::detail::winapi::DWORD_ dwDesiredAccess,
    autoboost::detail::winapi::BOOL_ bInheritHandle,
    autoboost::detail::winapi::LPCSTR_ lpName);
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
CreateFileMappingW(
    autoboost::detail::winapi::HANDLE_ hFile,
    ::_SECURITY_ATTRIBUTES* lpFileMappingAttributes,
    autoboost::detail::winapi::DWORD_ flProtect,
    autoboost::detail::winapi::DWORD_ dwMaximumSizeHigh,
    autoboost::detail::winapi::DWORD_ dwMaximumSizeLow,
    autoboost::detail::winapi::LPCWSTR_ lpName);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
OpenFileMappingW(
    autoboost::detail::winapi::DWORD_ dwDesiredAccess,
    autoboost::detail::winapi::BOOL_ bInheritHandle,
    autoboost::detail::winapi::LPCWSTR_ lpName);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::LPVOID_ WINAPI
MapViewOfFileEx(
    autoboost::detail::winapi::HANDLE_ hFileMappingObject,
    autoboost::detail::winapi::DWORD_ dwDesiredAccess,
    autoboost::detail::winapi::DWORD_ dwFileOffsetHigh,
    autoboost::detail::winapi::DWORD_ dwFileOffsetLow,
    autoboost::detail::winapi::SIZE_T_ dwNumberOfBytesToMap,
    autoboost::detail::winapi::LPVOID_ lpBaseAddress);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
FlushViewOfFile(
    autoboost::detail::winapi::LPCVOID_ lpBaseAddress,
    autoboost::detail::winapi::SIZE_T_ dwNumberOfBytesToFlush);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
UnmapViewOfFile(autoboost::detail::winapi::LPCVOID_ lpBaseAddress);
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {

#if !defined( AUTOBOOST_NO_ANSI_APIS )
using ::OpenFileMappingA;
#endif
using ::OpenFileMappingW;
using ::MapViewOfFileEx;
using ::FlushViewOfFile;
using ::UnmapViewOfFile;

#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_FORCEINLINE HANDLE_ CreateFileMappingA(
    HANDLE_ hFile,
    SECURITY_ATTRIBUTES_* lpFileMappingAttributes,
    DWORD_ flProtect,
    DWORD_ dwMaximumSizeHigh,
    DWORD_ dwMaximumSizeLow,
    LPCSTR_ lpName)
{
    return ::CreateFileMappingA(
        hFile,
        reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpFileMappingAttributes),
        flProtect,
        dwMaximumSizeHigh,
        dwMaximumSizeLow,
        lpName);
}
#endif

AUTOBOOST_FORCEINLINE HANDLE_ CreateFileMappingW(
    HANDLE_ hFile,
    ::_SECURITY_ATTRIBUTES* lpFileMappingAttributes,
    DWORD_ flProtect,
    DWORD_ dwMaximumSizeHigh,
    DWORD_ dwMaximumSizeLow,
    LPCWSTR_ lpName)
{
    return ::CreateFileMappingW(
        hFile,
        reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpFileMappingAttributes),
        flProtect,
        dwMaximumSizeHigh,
        dwMaximumSizeLow,
        lpName);
}

#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_FORCEINLINE HANDLE_ create_file_mapping(
    HANDLE_ hFile,
    SECURITY_ATTRIBUTES_* lpFileMappingAttributes,
    DWORD_ flProtect,
    DWORD_ dwMaximumSizeHigh,
    DWORD_ dwMaximumSizeLow,
    LPCSTR_ lpName)
{
    return ::CreateFileMappingA(
        hFile,
        reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpFileMappingAttributes),
        flProtect,
        dwMaximumSizeHigh,
        dwMaximumSizeLow,
        lpName);
}

AUTOBOOST_FORCEINLINE HANDLE_ open_file_mapping(DWORD_ dwDesiredAccess, BOOL_ bInheritHandle, LPCSTR_ lpName)
{
    return ::OpenFileMappingA(dwDesiredAccess, bInheritHandle, lpName);
}
#endif

AUTOBOOST_FORCEINLINE HANDLE_ create_file_mapping(
    HANDLE_ hFile,
    ::_SECURITY_ATTRIBUTES* lpFileMappingAttributes,
    DWORD_ flProtect,
    DWORD_ dwMaximumSizeHigh,
    DWORD_ dwMaximumSizeLow,
    LPCWSTR_ lpName)
{
    return ::CreateFileMappingW(
        hFile,
        reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpFileMappingAttributes),
        flProtect,
        dwMaximumSizeHigh,
        dwMaximumSizeLow,
        lpName);
}

AUTOBOOST_FORCEINLINE HANDLE_ open_file_mapping(DWORD_ dwDesiredAccess, BOOL_ bInheritHandle, LPCWSTR_ lpName)
{
    return ::OpenFileMappingW(dwDesiredAccess, bInheritHandle, lpName);
}

}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_FILE_MAPPING_HPP
