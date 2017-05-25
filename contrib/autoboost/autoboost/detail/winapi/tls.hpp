//  tls.hpp  --------------------------------------------------------------//

//  Copyright 2013 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_TLS_HPP
#define AUTOBOOST_DETAIL_WINAPI_TLS_HPP

#include <autoboost/detail/winapi/basic_types.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
#if !defined( UNDER_CE )
// Windows CE define TlsAlloc and TlsFree as inline functions in kfuncs.h
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
TlsAlloc(AUTOBOOST_DETAIL_WINAPI_VOID);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
TlsFree(autoboost::detail::winapi::DWORD_ dwTlsIndex);
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::LPVOID_ WINAPI
TlsGetValue(autoboost::detail::winapi::DWORD_ dwTlsIndex);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
TlsSetValue(
    autoboost::detail::winapi::DWORD_ dwTlsIndex,
    autoboost::detail::winapi::LPVOID_ lpTlsValue);
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {

using ::TlsAlloc;
using ::TlsFree;
using ::TlsGetValue;
using ::TlsSetValue;

#if defined( AUTOBOOST_USE_WINDOWS_H )
const DWORD_ TLS_OUT_OF_INDEXES_ = TLS_OUT_OF_INDEXES;
#else
const DWORD_ TLS_OUT_OF_INDEXES_ = 0xFFFFFFFF;
#endif

const DWORD_ tls_out_of_indexes = TLS_OUT_OF_INDEXES_;

}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_TLS_HPP
