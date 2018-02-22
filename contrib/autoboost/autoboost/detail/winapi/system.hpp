//  system.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright (c) Microsoft Corporation 2014
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_SYSTEM_HPP
#define AUTOBOOST_DETAIL_WINAPI_SYSTEM_HPP

#include <autoboost/detail/winapi/basic_types.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if defined(AUTOBOOST_MSVC)
#pragma warning(push)
// nonstandard extension used : nameless struct/union
#pragma warning(disable: 4201)
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
struct _SYSTEM_INFO;

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
GetSystemInfo(::_SYSTEM_INFO* lpSystemInfo);

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WINXP
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
GetNativeSystemInfo(::_SYSTEM_INFO* lpSystemInfo);
#endif
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {

typedef struct AUTOBOOST_DETAIL_WINAPI_MAY_ALIAS _SYSTEM_INFO {
    union {
        DWORD_ dwOemId;
        struct {
            WORD_ wProcessorArchitecture;
            WORD_ wReserved;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
    DWORD_ dwPageSize;
    LPVOID_ lpMinimumApplicationAddress;
    LPVOID_ lpMaximumApplicationAddress;
    DWORD_PTR_ dwActiveProcessorMask;
    DWORD_ dwNumberOfProcessors;
    DWORD_ dwProcessorType;
    DWORD_ dwAllocationGranularity;
    WORD_ wProcessorLevel;
    WORD_ wProcessorRevision;
} SYSTEM_INFO_, *LPSYSTEM_INFO_;

AUTOBOOST_FORCEINLINE VOID_ GetSystemInfo(LPSYSTEM_INFO_ lpSystemInfo)
{
    ::GetSystemInfo(reinterpret_cast< ::_SYSTEM_INFO* >(lpSystemInfo));
}

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WINXP
AUTOBOOST_FORCEINLINE VOID_ GetNativeSystemInfo(LPSYSTEM_INFO_ lpSystemInfo)
{
    ::GetNativeSystemInfo(reinterpret_cast< ::_SYSTEM_INFO* >(lpSystemInfo));
}
#endif

}
}
}

#if defined(AUTOBOOST_MSVC)
#pragma warning(pop)
#endif

#endif // AUTOBOOST_DETAIL_WINAPI_SYSTEM_HPP
