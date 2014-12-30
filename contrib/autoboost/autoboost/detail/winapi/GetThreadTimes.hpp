//  GetThreadTimes.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_GETTHREADTIMES_HPP
#define AUTOBOOST_DETAIL_WINAPI_GETTHREADTIMES_HPP

#include <autoboost/detail/winapi/time.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace autoboost {
namespace detail {
namespace winapi {
#if defined( AUTOBOOST_USE_WINDOWS_H )
    using ::GetThreadTimes;
#else
    extern "C" __declspec(dllimport) BOOL_ WINAPI
        GetThreadTimes(
            HANDLE_ hThread,
            LPFILETIME_ lpCreationTime,
            LPFILETIME_ lpExitTime,
            LPFILETIME_ lpKernelTime,
            LPFILETIME_ lpUserTime
        );
#endif
}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_GETTHREADTIMES_HPP
