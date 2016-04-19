//  apc.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_APC_HPP
#define AUTOBOOST_DETAIL_WINAPI_APC_HPP

#include <autoboost/detail/winapi/config.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_NT4

#include <autoboost/detail/winapi/basic_types.hpp>

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
typedef autoboost::detail::winapi::VOID_
(NTAPI *PAPCFUNC)(autoboost::detail::winapi::ULONG_PTR_ Parameter);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
QueueUserAPC(
    PAPCFUNC pfnAPC,
    autoboost::detail::winapi::HANDLE_ hThread,
    autoboost::detail::winapi::ULONG_PTR_ dwData);
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {
typedef ::PAPCFUNC PAPCFUNC_;
using ::QueueUserAPC;
}
}
}

#endif // AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_NT4

#endif // AUTOBOOST_DETAIL_WINAPI_APC_HPP
