//  srw_lock.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_SRW_LOCK_HPP
#define AUTOBOOST_DETAIL_WINAPI_SRW_LOCK_HPP

#include <autoboost/detail/winapi/config.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6

#include <autoboost/detail/winapi/basic_types.hpp>

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
struct _RTL_SRWLOCK;

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
InitializeSRWLock(::_RTL_SRWLOCK* SRWLock);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
ReleaseSRWLockExclusive(::_RTL_SRWLOCK* SRWLock);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
ReleaseSRWLockShared(::_RTL_SRWLOCK* SRWLock);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
AcquireSRWLockExclusive(::_RTL_SRWLOCK* SRWLock);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
AcquireSRWLockShared(::_RTL_SRWLOCK* SRWLock);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOLEAN_ WINAPI
TryAcquireSRWLockExclusive(::_RTL_SRWLOCK* SRWLock);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOLEAN_ WINAPI
TryAcquireSRWLockShared(::_RTL_SRWLOCK* SRWLock);
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {

typedef struct AUTOBOOST_DETAIL_WINAPI_MAY_ALIAS _RTL_SRWLOCK {
    PVOID_ Ptr;
} SRWLOCK_, *PSRWLOCK_;

#if defined( AUTOBOOST_USE_WINDOWS_H )
#define AUTOBOOST_DETAIL_WINAPI_SRWLOCK_INIT SRWLOCK_INIT
#else
#define AUTOBOOST_DETAIL_WINAPI_SRWLOCK_INIT {0}
#endif

AUTOBOOST_FORCEINLINE VOID_ InitializeSRWLock(PSRWLOCK_ SRWLock)
{
    ::InitializeSRWLock(reinterpret_cast< ::_RTL_SRWLOCK* >(SRWLock));
}

AUTOBOOST_FORCEINLINE VOID_ ReleaseSRWLockExclusive(PSRWLOCK_ SRWLock)
{
    ::ReleaseSRWLockExclusive(reinterpret_cast< ::_RTL_SRWLOCK* >(SRWLock));
}

AUTOBOOST_FORCEINLINE VOID_ ReleaseSRWLockShared(PSRWLOCK_ SRWLock)
{
    ::ReleaseSRWLockShared(reinterpret_cast< ::_RTL_SRWLOCK* >(SRWLock));
}

AUTOBOOST_FORCEINLINE VOID_ AcquireSRWLockExclusive(PSRWLOCK_ SRWLock)
{
    ::AcquireSRWLockExclusive(reinterpret_cast< ::_RTL_SRWLOCK* >(SRWLock));
}

AUTOBOOST_FORCEINLINE VOID_ AcquireSRWLockShared(PSRWLOCK_ SRWLock)
{
    ::AcquireSRWLockShared(reinterpret_cast< ::_RTL_SRWLOCK* >(SRWLock));
}

AUTOBOOST_FORCEINLINE BOOLEAN_ TryAcquireSRWLockExclusive(PSRWLOCK_ SRWLock)
{
    return ::TryAcquireSRWLockExclusive(reinterpret_cast< ::_RTL_SRWLOCK* >(SRWLock));
}

AUTOBOOST_FORCEINLINE BOOLEAN_ TryAcquireSRWLockShared(PSRWLOCK_ SRWLock)
{
    return ::TryAcquireSRWLockShared(reinterpret_cast< ::_RTL_SRWLOCK* >(SRWLock));
}

}
}
}

#endif // AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6

#endif // AUTOBOOST_DETAIL_WINAPI_SRW_LOCK_HPP
