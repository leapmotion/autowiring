//  condition_variable.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_CONDITION_VARIABLE_HPP
#define AUTOBOOST_DETAIL_WINAPI_CONDITION_VARIABLE_HPP

#include <autoboost/detail/winapi/config.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6

#include <autoboost/detail/winapi/basic_types.hpp>

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
struct _RTL_CONDITION_VARIABLE;
struct _RTL_CRITICAL_SECTION;
struct _RTL_SRWLOCK;

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
InitializeConditionVariable(::_RTL_CONDITION_VARIABLE* ConditionVariable);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
WakeConditionVariable(::_RTL_CONDITION_VARIABLE* ConditionVariable);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
WakeAllConditionVariable(::_RTL_CONDITION_VARIABLE* ConditionVariable);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
SleepConditionVariableCS(
    ::_RTL_CONDITION_VARIABLE* ConditionVariable,
    ::_RTL_CRITICAL_SECTION* CriticalSection,
    autoboost::detail::winapi::DWORD_ dwMilliseconds);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
SleepConditionVariableSRW(
    ::_RTL_CONDITION_VARIABLE* ConditionVariable,
    ::_RTL_SRWLOCK* SRWLock,
    autoboost::detail::winapi::DWORD_ dwMilliseconds,
    autoboost::detail::winapi::ULONG_ Flags);
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {

typedef struct AUTOBOOST_DETAIL_WINAPI_MAY_ALIAS _RTL_CONDITION_VARIABLE {
    PVOID_ Ptr;
} CONDITION_VARIABLE_, *PCONDITION_VARIABLE_;

#if defined( AUTOBOOST_USE_WINDOWS_H )
#define AUTOBOOST_DETAIL_WINAPI_CONDITION_VARIABLE_INIT CONDITION_VARIABLE_INIT
#else
#define AUTOBOOST_DETAIL_WINAPI_CONDITION_VARIABLE {0}
#endif

struct _RTL_CRITICAL_SECTION;
struct _RTL_SRWLOCK;

AUTOBOOST_FORCEINLINE VOID_ InitializeConditionVariable(PCONDITION_VARIABLE_ ConditionVariable)
{
    ::InitializeConditionVariable(reinterpret_cast< ::_RTL_CONDITION_VARIABLE* >(ConditionVariable));
}

AUTOBOOST_FORCEINLINE VOID_ WakeConditionVariable(PCONDITION_VARIABLE_ ConditionVariable)
{
    ::WakeConditionVariable(reinterpret_cast< ::_RTL_CONDITION_VARIABLE* >(ConditionVariable));
}

AUTOBOOST_FORCEINLINE VOID_ WakeAllConditionVariable(PCONDITION_VARIABLE_ ConditionVariable)
{
    ::WakeAllConditionVariable(reinterpret_cast< ::_RTL_CONDITION_VARIABLE* >(ConditionVariable));
}

AUTOBOOST_FORCEINLINE BOOL_ SleepConditionVariableCS(
    PCONDITION_VARIABLE_ ConditionVariable,
    _RTL_CRITICAL_SECTION* CriticalSection,
    DWORD_ dwMilliseconds)
{
    return ::SleepConditionVariableCS(
        reinterpret_cast< ::_RTL_CONDITION_VARIABLE* >(ConditionVariable),
        reinterpret_cast< ::_RTL_CRITICAL_SECTION* >(CriticalSection),
        dwMilliseconds);
}

AUTOBOOST_FORCEINLINE BOOL_ SleepConditionVariableSRW(
    PCONDITION_VARIABLE_ ConditionVariable,
    _RTL_SRWLOCK* SRWLock,
    DWORD_ dwMilliseconds,
    ULONG_ Flags)
{
    return ::SleepConditionVariableSRW(
        reinterpret_cast< ::_RTL_CONDITION_VARIABLE* >(ConditionVariable),
        reinterpret_cast< ::_RTL_SRWLOCK* >(SRWLock),
        dwMilliseconds,
        Flags);
}

#if defined( AUTOBOOST_USE_WINDOWS_H )
const ULONG_ CONDITION_VARIABLE_LOCKMODE_SHARED_ = CONDITION_VARIABLE_LOCKMODE_SHARED;
#else // defined( AUTOBOOST_USE_WINDOWS_H )
const ULONG_ CONDITION_VARIABLE_LOCKMODE_SHARED_ = 0x00000001;
#endif // defined( AUTOBOOST_USE_WINDOWS_H )

const ULONG_ condition_variable_lockmode_shared = CONDITION_VARIABLE_LOCKMODE_SHARED_;

}
}
}

#endif // AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6

#endif // AUTOBOOST_DETAIL_WINAPI_CONDITION_VARIABLE_HPP
