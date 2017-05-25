//  local_memory.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_LOCAL_MEMORY_HPP
#define AUTOBOOST_DETAIL_WINAPI_LOCAL_MEMORY_HPP

#include <autoboost/detail/winapi/basic_types.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
namespace autoboost { namespace detail { namespace winapi {
typedef HANDLE_ HLOCAL_;
}}}

extern "C" {
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HLOCAL_ WINAPI
LocalAlloc(
    autoboost::detail::winapi::UINT_ uFlags,
    autoboost::detail::winapi::SIZE_T_ uBytes);
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HLOCAL_ WINAPI
LocalReAlloc(
    autoboost::detail::winapi::HLOCAL_ hMem,
    autoboost::detail::winapi::SIZE_T_ uBytes,
    autoboost::detail::winapi::UINT_ uFlags);
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HLOCAL_ WINAPI LocalFree(autoboost::detail::winapi::HLOCAL_ hMem);
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {
#if defined( AUTOBOOST_USE_WINDOWS_H )
typedef ::HLOCAL HLOCAL_;
#endif
using ::LocalAlloc;
using ::LocalReAlloc;
using ::LocalFree;
}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_LOCAL_MEMORY_HPP
