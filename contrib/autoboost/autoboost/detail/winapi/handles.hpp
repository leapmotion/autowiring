//  memory.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_HANDLES_HPP
#define AUTOBOOST_DETAIL_WINAPI_HANDLES_HPP

#include <autoboost/detail/winapi/basic_types.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace autoboost
{
namespace detail
{
namespace winapi
{
#if defined( AUTOBOOST_USE_WINDOWS_H )
    using ::CloseHandle;
    using ::DuplicateHandle;

    const DWORD_ duplicate_close_source = DUPLICATE_CLOSE_SOURCE;
    const DWORD_ duplicate_same_access = DUPLICATE_SAME_ACCESS;
    const HANDLE_ invalid_handle_value = INVALID_HANDLE_VALUE;
#else
extern "C" {
    __declspec(dllimport) int __stdcall
        CloseHandle(void*);
    __declspec(dllimport) int __stdcall
        DuplicateHandle(void*,void*,void*,void**,unsigned long,int,unsigned long);
}
    const DWORD_ duplicate_close_source = 1;
    const DWORD_ duplicate_same_access = 2;
    const HANDLE_ invalid_handle_value = (HANDLE_)(-1);
#endif
}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_HANDLES_HPP
