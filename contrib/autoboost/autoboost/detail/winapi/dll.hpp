//  dll.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2014 Renato Tegon Forti, Antony Polukhin
//  Copyright 2015 Andrey Semashev
//  Copyright 2015 Antony Polukhin

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_DLL_HPP
#define AUTOBOOST_DETAIL_WINAPI_DLL_HPP

#include <autoboost/detail/winapi/basic_types.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
namespace autoboost { namespace detail { namespace winapi {
#ifdef _WIN64
typedef INT_PTR_ (WINAPI *FARPROC_)();
typedef INT_PTR_ (WINAPI *NEARPROC_)();
typedef INT_PTR_ (WINAPI *PROC_)();
#else
typedef int (WINAPI *FARPROC_)();
typedef int (WINAPI *NEARPROC_)();
typedef int (WINAPI *PROC_)();
#endif // _WIN64
}}} // namespace autoboost::detail::winapi

#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HMODULE_ WINAPI
LoadLibraryA(autoboost::detail::winapi::LPCSTR_ lpFileName);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HMODULE_ WINAPI
LoadLibraryExA(
    autoboost::detail::winapi::LPCSTR_ lpFileName,
    autoboost::detail::winapi::HANDLE_ hFile,
    autoboost::detail::winapi::DWORD_ dwFlags
);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HMODULE_ WINAPI
GetModuleHandleA(autoboost::detail::winapi::LPCSTR_ lpFileName);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
GetModuleFileNameA(
    autoboost::detail::winapi::HMODULE_ hModule,
    autoboost::detail::winapi::LPSTR_ lpFilename,
    autoboost::detail::winapi::DWORD_ nSize
);
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HMODULE_ WINAPI
LoadLibraryW(autoboost::detail::winapi::LPCWSTR_ lpFileName);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HMODULE_ WINAPI
LoadLibraryExW(
    autoboost::detail::winapi::LPCWSTR_ lpFileName,
    autoboost::detail::winapi::HANDLE_ hFile,
    autoboost::detail::winapi::DWORD_ dwFlags
);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HMODULE_ WINAPI
GetModuleHandleW(autoboost::detail::winapi::LPCWSTR_ lpFileName);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
GetModuleFileNameW(
    autoboost::detail::winapi::HMODULE_ hModule,
    autoboost::detail::winapi::LPWSTR_ lpFilename,
    autoboost::detail::winapi::DWORD_ nSize
);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
FreeLibrary(autoboost::detail::winapi::HMODULE_ hModule);

#if !defined( UNDER_CE )
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::FARPROC_ WINAPI
GetProcAddress(autoboost::detail::winapi::HMODULE_ hModule, autoboost::detail::winapi::LPCSTR_ lpProcName);
#else
// On Windows CE there are two functions: GetProcAddressA (since Windows CE 3.0) and GetProcAddressW.
// GetProcAddress is a macro that is _always_ defined to GetProcAddressW.
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::FARPROC_ WINAPI
GetProcAddressA(autoboost::detail::winapi::HMODULE_ hModule, autoboost::detail::winapi::LPCSTR_ lpProcName);
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::FARPROC_ WINAPI
GetProcAddressW(autoboost::detail::winapi::HMODULE_ hModule, autoboost::detail::winapi::LPCWSTR_ lpProcName);
#endif

struct _MEMORY_BASIC_INFORMATION;

#if !defined( AUTOBOOST_WINAPI_IS_MINGW )
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::SIZE_T_ WINAPI
VirtualQuery(
    autoboost::detail::winapi::LPCVOID_ lpAddress,
    ::_MEMORY_BASIC_INFORMATION* lpBuffer,
    autoboost::detail::winapi::ULONG_PTR_ dwLength
);
#else // !defined( AUTOBOOST_WINAPI_IS_MINGW )
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
VirtualQuery(
    autoboost::detail::winapi::LPCVOID_ lpAddress,
    ::_MEMORY_BASIC_INFORMATION* lpBuffer,
    autoboost::detail::winapi::DWORD_ dwLength
);
#endif // !defined( AUTOBOOST_WINAPI_IS_MINGW )
} // extern "C"
#endif // #if !defined( AUTOBOOST_USE_WINDOWS_H )

namespace autoboost {
namespace detail {
namespace winapi {

typedef struct AUTOBOOST_DETAIL_WINAPI_MAY_ALIAS MEMORY_BASIC_INFORMATION_ {
    PVOID_  BaseAddress;
    PVOID_  AllocationBase;
    DWORD_  AllocationProtect;
    SIZE_T_ RegionSize;
    DWORD_  State;
    DWORD_  Protect;
    DWORD_  Type;
} *PMEMORY_BASIC_INFORMATION_;

#if defined( AUTOBOOST_USE_WINDOWS_H )
typedef ::FARPROC FARPROC_;
typedef ::NEARPROC NEARPROC_;
typedef ::PROC PROC_;

const DWORD_ DONT_RESOLVE_DLL_REFERENCES_           = DONT_RESOLVE_DLL_REFERENCES;
const DWORD_ LOAD_WITH_ALTERED_SEARCH_PATH_         = LOAD_WITH_ALTERED_SEARCH_PATH;
#else // defined( AUTOBOOST_USE_WINDOWS_H )
const DWORD_ DONT_RESOLVE_DLL_REFERENCES_           = 0x00000001;
const DWORD_ LOAD_WITH_ALTERED_SEARCH_PATH_         = 0x00000008;
#endif // defined( AUTOBOOST_USE_WINDOWS_H )

// This one is not defined by MinGW
const DWORD_ LOAD_IGNORE_CODE_AUTHZ_LEVEL_          = 0x00000010;

#if !defined( AUTOBOOST_NO_ANSI_APIS )
using ::LoadLibraryA;
using ::LoadLibraryExA;
using ::GetModuleHandleA;
using ::GetModuleFileNameA;
#endif // !defined( AUTOBOOST_NO_ANSI_APIS )
using ::LoadLibraryW;
using ::LoadLibraryExW;
using ::GetModuleHandleW;
using ::GetModuleFileNameW;
using ::FreeLibrary;

#if !defined( UNDER_CE )
// For backward compatibility, don't use directly. Use get_proc_address instead.
using ::GetProcAddress;
#else
using ::GetProcAddressA;
using ::GetProcAddressW;
#endif

AUTOBOOST_FORCEINLINE FARPROC_ get_proc_address(HMODULE_ hModule, LPCSTR_ lpProcName)
{
#if !defined( UNDER_CE )
    return ::GetProcAddress(hModule, lpProcName);
#else
    return ::GetProcAddressA(hModule, lpProcName);
#endif
}

AUTOBOOST_FORCEINLINE SIZE_T_ VirtualQuery(LPCVOID_ lpAddress, MEMORY_BASIC_INFORMATION_* lpBuffer, ULONG_PTR_ dwLength)
{
    return ::VirtualQuery(lpAddress, reinterpret_cast< ::_MEMORY_BASIC_INFORMATION* >(lpBuffer), dwLength);
}

#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_FORCEINLINE HMODULE_ load_library(LPCSTR_ lpFileName)
{
    return ::LoadLibraryA(lpFileName);
}

AUTOBOOST_FORCEINLINE HMODULE_ load_library_ex(LPCSTR_ lpFileName, HANDLE_ hFile, DWORD_ dwFlags)
{
    return ::LoadLibraryExA(lpFileName, hFile, dwFlags);
}

AUTOBOOST_FORCEINLINE HMODULE_ get_module_handle(LPCSTR_ lpFileName)
{
    return ::GetModuleHandleA(lpFileName);
}

AUTOBOOST_FORCEINLINE DWORD_ get_module_file_name(HMODULE_ hModule, LPSTR_ lpFilename, DWORD_ nSize)
{
    return ::GetModuleFileNameA(hModule, lpFilename, nSize);
}
#endif // #if !defined( AUTOBOOST_NO_ANSI_APIS )

AUTOBOOST_FORCEINLINE HMODULE_ load_library(LPCWSTR_ lpFileName)
{
    return ::LoadLibraryW(lpFileName);
}

AUTOBOOST_FORCEINLINE HMODULE_ load_library_ex(LPCWSTR_ lpFileName, HANDLE_ hFile, DWORD_ dwFlags)
{
    return ::LoadLibraryExW(lpFileName, hFile, dwFlags);
}

AUTOBOOST_FORCEINLINE HMODULE_ get_module_handle(LPCWSTR_ lpFileName)
{
    return ::GetModuleHandleW(lpFileName);
}

AUTOBOOST_FORCEINLINE DWORD_ get_module_file_name(HMODULE_ hModule, LPWSTR_ lpFilename, DWORD_ nSize)
{
    return ::GetModuleFileNameW(hModule, lpFilename, nSize);
}

} // namespace winapi
} // namespace detail
} // namespace autoboost

#endif // AUTOBOOST_DETAIL_WINAPI_DLL_HPP
