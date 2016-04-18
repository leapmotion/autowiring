//  timers.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_TIMERS_HPP
#define AUTOBOOST_DETAIL_WINAPI_TIMERS_HPP

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
    using ::QueryPerformanceCounter;
    using ::QueryPerformanceFrequency;
#else
extern "C" {
    __declspec(dllimport) BOOL_ WINAPI
        QueryPerformanceCounter(
            LARGE_INTEGER_ *lpPerformanceCount
        );

    __declspec(dllimport) BOOL_ WINAPI
        QueryPerformanceFrequency(
            LARGE_INTEGER_ *lpFrequency
        );
}
#endif
}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_TIMERS_HPP
