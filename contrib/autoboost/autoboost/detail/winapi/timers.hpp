//  timers.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_TIMERS_HPP
#define AUTOBOOST_DETAIL_WINAPI_TIMERS_HPP

#include <autoboost/detail/winapi/basic_types.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
QueryPerformanceCounter(::_LARGE_INTEGER* lpPerformanceCount);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
QueryPerformanceFrequency(::_LARGE_INTEGER* lpFrequency);
}
#endif


namespace autoboost {
namespace detail {
namespace winapi {

AUTOBOOST_FORCEINLINE BOOL_ QueryPerformanceCounter(LARGE_INTEGER_* lpPerformanceCount)
{
    return ::QueryPerformanceCounter(reinterpret_cast< ::_LARGE_INTEGER* >(lpPerformanceCount));
}

AUTOBOOST_FORCEINLINE BOOL_ QueryPerformanceFrequency(LARGE_INTEGER_* lpFrequency)
{
    return ::QueryPerformanceFrequency(reinterpret_cast< ::_LARGE_INTEGER* >(lpFrequency));
}

}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_TIMERS_HPP
