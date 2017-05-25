//  heap_memory.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_HEAP_MEMORY_HPP
#define AUTOBOOST_DETAIL_WINAPI_HEAP_MEMORY_HPP

#include <autoboost/detail/winapi/basic_types.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
#undef HeapAlloc
extern "C" {
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
GetProcessHeap(AUTOBOOST_DETAIL_WINAPI_VOID);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
GetProcessHeaps(autoboost::detail::winapi::DWORD_ NumberOfHeaps, autoboost::detail::winapi::PHANDLE_ ProcessHeaps);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
HeapCreate(
    autoboost::detail::winapi::DWORD_ flOptions,
    autoboost::detail::winapi::SIZE_T_ dwInitialSize,
    autoboost::detail::winapi::SIZE_T_ dwMaximumSize);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
HeapDestroy(autoboost::detail::winapi::HANDLE_ hHeap);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::LPVOID_ WINAPI
HeapAlloc(
    autoboost::detail::winapi::HANDLE_ hHeap,
    autoboost::detail::winapi::DWORD_ dwFlags,
    autoboost::detail::winapi::SIZE_T_ dwBytes);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::LPVOID_ WINAPI
HeapReAlloc(
    autoboost::detail::winapi::HANDLE_ hHeap,
    autoboost::detail::winapi::DWORD_ dwFlags,
    autoboost::detail::winapi::LPVOID_ lpMem,
    autoboost::detail::winapi::SIZE_T_ dwBytes);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
HeapFree(
    autoboost::detail::winapi::HANDLE_ hHeap,
    autoboost::detail::winapi::DWORD_ dwFlags,
    autoboost::detail::winapi::LPVOID_ lpMem);
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {
using ::GetProcessHeap;
using ::GetProcessHeaps;
using ::HeapCreate;
using ::HeapDestroy;
using ::HeapAlloc;
using ::HeapReAlloc;
using ::HeapFree;
}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_HEAP_MEMORY_HPP
