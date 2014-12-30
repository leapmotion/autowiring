//  GetProcessTimes.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_GETPROCESSTIMES_HPP
#define AUTOBOOST_DETAIL_WINAPI_GETPROCESSTIMES_HPP

#include <autoboost/detail/winapi/time.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace autoboost {
namespace detail {
namespace winapi {
#if !defined(UNDER_CE)  // Windows CE does not define GetProcessTimes
#if defined( AUTOBOOST_USE_WINDOWS_H )
    using ::GetProcessTimes;
#else
    extern "C" __declspec(dllimport) BOOL_ WINAPI
        GetProcessTimes(
            HANDLE_ hProcess,
            LPFILETIME_ lpCreationTime,
            LPFILETIME_ lpExitTime,
            LPFILETIME_ lpKernelTime,
            LPFILETIME_ lpUserTime
        );
#endif
#endif
}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_GETPROCESSTIMES_HPP
