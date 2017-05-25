//  handles.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_HANDLES_HPP
#define AUTOBOOST_DETAIL_WINAPI_HANDLES_HPP

#include <autoboost/detail/winapi/basic_types.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
CloseHandle(autoboost::detail::winapi::HANDLE_ handle);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
DuplicateHandle(
    autoboost::detail::winapi::HANDLE_ hSourceProcessHandle,
    autoboost::detail::winapi::HANDLE_ hSourceHandle,
    autoboost::detail::winapi::HANDLE_ hTargetProcessHandle,
    autoboost::detail::winapi::HANDLE_* lpTargetHandle,
    autoboost::detail::winapi::DWORD_ dwDesiredAccess,
    autoboost::detail::winapi::BOOL_ bInheritHandle,
    autoboost::detail::winapi::DWORD_ dwOptions);
}

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN10
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
CompareObjectHandles(
    autoboost::detail::winapi::HANDLE_ hFirstObjectHandle,
    autoboost::detail::winapi::HANDLE_ hSecondObjectHandle);
#endif

#endif

namespace autoboost {
namespace detail {
namespace winapi {

using ::CloseHandle;
using ::DuplicateHandle;

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN10
using ::CompareObjectHandles;
#endif

#if defined( AUTOBOOST_USE_WINDOWS_H )
const DWORD_ DUPLICATE_CLOSE_SOURCE_ = DUPLICATE_CLOSE_SOURCE;
const DWORD_ DUPLICATE_SAME_ACCESS_ = DUPLICATE_SAME_ACCESS;
const HANDLE_ INVALID_HANDLE_VALUE_ = INVALID_HANDLE_VALUE;
#else
const DWORD_ DUPLICATE_CLOSE_SOURCE_ = 1;
const DWORD_ DUPLICATE_SAME_ACCESS_ = 2;
const HANDLE_ INVALID_HANDLE_VALUE_ = (HANDLE_)(-1);
#endif

const DWORD_ duplicate_close_source = DUPLICATE_CLOSE_SOURCE_;
const DWORD_ duplicate_same_access = DUPLICATE_SAME_ACCESS_;
const HANDLE_ invalid_handle_value = INVALID_HANDLE_VALUE_;

}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_HANDLES_HPP
