//  LocalFree.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_LOCALFREE_HPP
#define AUTOBOOST_DETAIL_WINAPI_LOCALFREE_HPP

#include <autoboost/detail/winapi/basic_types.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace autoboost {
namespace detail {
namespace winapi {
#if defined( AUTOBOOST_USE_WINDOWS_H )
    typedef HANDLE_ HLOCAL_;

    using ::LocalFree;
#else
    extern "C" typedef HANDLE_ HLOCAL_;
    extern "C" __declspec(dllimport) HLOCAL_ WINAPI
        LocalFree(HLOCAL_ hMem);
#endif
}
}
}
#endif // AUTOBOOST_DETAIL_WINAPI_LOCALFREE_HPP
