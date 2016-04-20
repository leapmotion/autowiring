//  GetCurrentThread.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_GETCURRENTTHREAD_HPP
#define AUTOBOOST_DETAIL_WINAPI_GETCURRENTTHREAD_HPP

#include <autoboost/detail/winapi/basic_types.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace autoboost {
namespace detail {
namespace winapi {
#if defined( UNDER_CE )
// Windows CE define GetCurrentThread as an inline function in kfuncs.h
inline HANDLE_ GetCurrentThread()
{
  return ::GetCurrentThread();
}
#else
#if defined( AUTOBOOST_USE_WINDOWS_H )
    using ::GetCurrentThread;
#else
    extern "C" __declspec(dllimport) HANDLE_ WINAPI GetCurrentThread();
#endif
#endif
}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_GETCURRENTTHREAD_HPP
