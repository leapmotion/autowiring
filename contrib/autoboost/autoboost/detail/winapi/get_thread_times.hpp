//  get_thread_times.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_GET_THREAD_TIMES_HPP
#define AUTOBOOST_DETAIL_WINAPI_GET_THREAD_TIMES_HPP

#include <autoboost/detail/winapi/basic_types.hpp>
#include <autoboost/detail/winapi/time.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
GetThreadTimes(
    autoboost::detail::winapi::HANDLE_ hThread,
    ::_FILETIME* lpCreationTime,
    ::_FILETIME* lpExitTime,
    ::_FILETIME* lpKernelTime,
    ::_FILETIME* lpUserTime);
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {

AUTOBOOST_FORCEINLINE BOOL_ GetThreadTimes(
    HANDLE_ hThread,
    LPFILETIME_ lpCreationTime,
    LPFILETIME_ lpExitTime,
    LPFILETIME_ lpKernelTime,
    LPFILETIME_ lpUserTime)
{
    return ::GetThreadTimes(
        hThread,
        reinterpret_cast< ::_FILETIME* >(lpCreationTime),
        reinterpret_cast< ::_FILETIME* >(lpExitTime),
        reinterpret_cast< ::_FILETIME* >(lpKernelTime),
        reinterpret_cast< ::_FILETIME* >(lpUserTime));
}

}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_GET_THREAD_TIMES_HPP
