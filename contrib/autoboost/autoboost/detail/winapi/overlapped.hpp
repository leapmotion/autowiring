//  overlapped.hpp  --------------------------------------------------------------//

//  Copyright 2016 Klemens D. Morgenstern

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#ifndef AUTOBOOST_DETAIL_WINAPI_OVERLAPPED_HPP_
#define AUTOBOOST_DETAIL_WINAPI_OVERLAPPED_HPP_

#include <autoboost/detail/winapi/basic_types.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
struct _OVERLAPPED;
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {

#ifdef AUTOBOOST_MSVC
#pragma warning(push)
#pragma warning(disable:4201) // nonstandard extension used : nameless struct/union
#endif

typedef struct AUTOBOOST_DETAIL_WINAPI_MAY_ALIAS _OVERLAPPED {
    ULONG_PTR_ Internal;
    ULONG_PTR_ InternalHigh;
    union {
        struct {
            DWORD_ Offset;
            DWORD_ OffsetHigh;
        };
        PVOID_  Pointer;
    };
    HANDLE_    hEvent;
} OVERLAPPED_, *LPOVERLAPPED_;

#ifdef AUTOBOOST_MSVC
#pragma warning(pop)
#endif

}}}

#endif // AUTOBOOST_DETAIL_WINAPI_OVERLAPPED_HPP_
