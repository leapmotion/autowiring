//  GetProcessTimes.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_GETPROCESSTIMES_HPP
#define AUTOBOOST_DETAIL_WINAPI_GETPROCESSTIMES_HPP

#include <autoboost/detail/winapi/config.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

// Windows CE does not define GetProcessTimes
#if !defined( UNDER_CE )

#include <autoboost/detail/winapi/basic_types.hpp>
#include <autoboost/detail/winapi/time.hpp>

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
GetProcessTimes(
    autoboost::detail::winapi::HANDLE_ hProcess,
    ::_FILETIME* lpCreationTime,
    ::_FILETIME* lpExitTime,
    ::_FILETIME* lpKernelTime,
    ::_FILETIME* lpUserTime);
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {

AUTOBOOST_FORCEINLINE BOOL_ GetProcessTimes(
    HANDLE_ hProcess,
    LPFILETIME_ lpCreationTime,
    LPFILETIME_ lpExitTime,
    LPFILETIME_ lpKernelTime,
    LPFILETIME_ lpUserTime)
{
    return ::GetProcessTimes(
        hProcess,
        reinterpret_cast< ::_FILETIME* >(lpCreationTime),
        reinterpret_cast< ::_FILETIME* >(lpExitTime),
        reinterpret_cast< ::_FILETIME* >(lpKernelTime),
        reinterpret_cast< ::_FILETIME* >(lpUserTime));
}

}
}
}

#endif // !defined( UNDER_CE )
#endif // AUTOBOOST_DETAIL_WINAPI_GETPROCESSTIMES_HPP
