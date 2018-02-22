//  critical_section.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_CRITICAL_SECTION_HPP
#define AUTOBOOST_DETAIL_WINAPI_CRITICAL_SECTION_HPP

#include <autoboost/detail/winapi/basic_types.hpp>
#include <autoboost/detail/winapi/detail/cast_ptr.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )

extern "C" {
#if !defined( AUTOBOOST_WINAPI_IS_MINGW )
struct _RTL_CRITICAL_SECTION;

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
InitializeCriticalSection(::_RTL_CRITICAL_SECTION* lpCriticalSection);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
EnterCriticalSection(::_RTL_CRITICAL_SECTION* lpCriticalSection);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
LeaveCriticalSection(::_RTL_CRITICAL_SECTION* lpCriticalSection);

#if AUTOBOOST_USE_WINAPI_VERSION >= 0x0403
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
InitializeCriticalSectionAndSpinCount(
    ::_RTL_CRITICAL_SECTION* lpCriticalSection,
    autoboost::detail::winapi::DWORD_ dwSpinCount);

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
InitializeCriticalSectionEx(
    ::_RTL_CRITICAL_SECTION* lpCriticalSection,
    autoboost::detail::winapi::DWORD_ dwSpinCount,
    autoboost::detail::winapi::DWORD_ Flags);
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
SetCriticalSectionSpinCount(
    ::_RTL_CRITICAL_SECTION* lpCriticalSection,
    autoboost::detail::winapi::DWORD_ dwSpinCount);
#endif

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_NT4
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
TryEnterCriticalSection(::_RTL_CRITICAL_SECTION* lpCriticalSection);
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
DeleteCriticalSection(::_RTL_CRITICAL_SECTION* lpCriticalSection);

#else // defined( AUTOBOOST_WINAPI_IS_MINGW )

// MinGW uses a different name for the structure
struct _CRITICAL_SECTION;

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
InitializeCriticalSection(::_CRITICAL_SECTION* lpCriticalSection);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
EnterCriticalSection(::_CRITICAL_SECTION* lpCriticalSection);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
LeaveCriticalSection(::_CRITICAL_SECTION* lpCriticalSection);

#if AUTOBOOST_USE_WINAPI_VERSION >= 0x0403
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
InitializeCriticalSectionAndSpinCount(
    ::_CRITICAL_SECTION* lpCriticalSection,
    autoboost::detail::winapi::DWORD_ dwSpinCount);

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
InitializeCriticalSectionEx(
    ::_CRITICAL_SECTION* lpCriticalSection,
    autoboost::detail::winapi::DWORD_ dwSpinCount,
    autoboost::detail::winapi::DWORD_ Flags);
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
SetCriticalSectionSpinCount(
    ::_CRITICAL_SECTION* lpCriticalSection,
    autoboost::detail::winapi::DWORD_ dwSpinCount);
#endif

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_NT4
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
TryEnterCriticalSection(::_CRITICAL_SECTION* lpCriticalSection);
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::VOID_ WINAPI
DeleteCriticalSection(::_CRITICAL_SECTION* lpCriticalSection);

#endif // defined( AUTOBOOST_WINAPI_IS_MINGW )
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {

struct _RTL_CRITICAL_SECTION_DEBUG;

#pragma pack(push, 8)

typedef struct AUTOBOOST_DETAIL_WINAPI_MAY_ALIAS _RTL_CRITICAL_SECTION {
    _RTL_CRITICAL_SECTION_DEBUG* DebugInfo;
    LONG_ LockCount;
    LONG_ RecursionCount;
    HANDLE_ OwningThread;
    HANDLE_ LockSemaphore;
    ULONG_PTR_ SpinCount;
} CRITICAL_SECTION_, *PCRITICAL_SECTION_;

#pragma pack(pop)

AUTOBOOST_FORCEINLINE VOID_ InitializeCriticalSection(CRITICAL_SECTION_* lpCriticalSection)
{
    ::InitializeCriticalSection(winapi::detail::cast_ptr(lpCriticalSection));
}

AUTOBOOST_FORCEINLINE VOID_ EnterCriticalSection(CRITICAL_SECTION_* lpCriticalSection)
{
    ::EnterCriticalSection(winapi::detail::cast_ptr(lpCriticalSection));
}

AUTOBOOST_FORCEINLINE VOID_ LeaveCriticalSection(CRITICAL_SECTION_* lpCriticalSection)
{
    ::LeaveCriticalSection(winapi::detail::cast_ptr(lpCriticalSection));
}

#if AUTOBOOST_USE_WINAPI_VERSION >= 0x0403
AUTOBOOST_FORCEINLINE BOOL_ InitializeCriticalSectionAndSpinCount(CRITICAL_SECTION_* lpCriticalSection, DWORD_ dwSpinCount)
{
    return ::InitializeCriticalSectionAndSpinCount(winapi::detail::cast_ptr(lpCriticalSection), dwSpinCount);
}

// CRITICAL_SECTION_NO_DEBUG_INFO is defined for WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
const DWORD_ CRITICAL_SECTION_NO_DEBUG_INFO_ = 0x01000000;
const DWORD_ CRITICAL_SECTION_FLAG_NO_DEBUG_INFO_ = CRITICAL_SECTION_NO_DEBUG_INFO_;
const DWORD_ CRITICAL_SECTION_FLAG_DYNAMIC_SPIN_ = 0x02000000; // undocumented
const DWORD_ CRITICAL_SECTION_FLAG_STATIC_INIT_ = 0x04000000; // undocumented

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
AUTOBOOST_FORCEINLINE BOOL_ InitializeCriticalSectionEx(CRITICAL_SECTION_* lpCriticalSection, DWORD_ dwSpinCount, DWORD_ Flags)
{
    return ::InitializeCriticalSectionEx(winapi::detail::cast_ptr(lpCriticalSection), dwSpinCount, Flags);
}
#endif // AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6

AUTOBOOST_FORCEINLINE DWORD_ SetCriticalSectionSpinCount(CRITICAL_SECTION_* lpCriticalSection, DWORD_ dwSpinCount)
{
    return ::SetCriticalSectionSpinCount(winapi::detail::cast_ptr(lpCriticalSection), dwSpinCount);
}
#endif // AUTOBOOST_USE_WINAPI_VERSION >= 0x0403

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_NT4
AUTOBOOST_FORCEINLINE BOOL_ TryEnterCriticalSection(CRITICAL_SECTION_* lpCriticalSection)
{
    return ::TryEnterCriticalSection(winapi::detail::cast_ptr(lpCriticalSection));
}
#endif // AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_NT4

AUTOBOOST_FORCEINLINE VOID_ DeleteCriticalSection(CRITICAL_SECTION_* lpCriticalSection)
{
    ::DeleteCriticalSection(winapi::detail::cast_ptr(lpCriticalSection));
}

}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_CRITICAL_SECTION_HPP
