//  dbghelp.hpp  --------------------------------------------------------------//

//  Copyright 2015 Klemens Morgenstern
//  Copyright 2016 Jorge Lodos
//  Copyright 2016 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_DBGHELP_HPP
#define AUTOBOOST_DETAIL_WINAPI_DBGHELP_HPP

#include <autoboost/detail/winapi/basic_types.hpp>

#if defined( AUTOBOOST_USE_WINDOWS_H )
#if !defined( AUTOBOOST_WINAPI_IS_MINGW )
#include <dbghelp.h>
#else
// In MinGW there is no dbghelp.h but an older imagehlp.h header defines some of the symbols from it.
// Note that the user has to link with libimagehlp.a instead of libdbghelp.a for it to work.
#include <imagehlp.h>
#endif
#endif

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

// Some symbols declared below are not present in all versions of Windows SDK, MinGW and MinGW-w64.
// dbghelp.h/imagehlp.h define the API_VERSION_NUMBER macro which we use to detect its version.
// When the macro is not available we can only guess based on the compiler version or SDK type.
#if defined(API_VERSION_NUMBER)
#if API_VERSION_NUMBER >= 11
// UnDecorateSymbolNameW available since Windows SDK 6.0A and MinGW-w64 (as of 2016-02-14)
#define AUTOBOOST_DETAIL_WINAPI_HAS_UNDECORATESYMBOLNAMEW
#endif
#elif defined(_MSC_VER) && _MSC_VER >= 1500
// Until MSVC 9.0 Windows SDK was bundled in Visual Studio and didn't have UnDecorateSymbolNameW.
// Supposedly, Windows SDK 6.0A was the first standalone one and it is used with MSVC 9.0.
#define AUTOBOOST_DETAIL_WINAPI_HAS_UNDECORATESYMBOLNAMEW
#elif !defined(AUTOBOOST_WINAPI_IS_MINGW)
// MinGW does not provide UnDecorateSymbolNameW (as of 2016-02-14)
#define AUTOBOOST_DETAIL_WINAPI_HAS_UNDECORATESYMBOLNAMEW
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {

struct API_VERSION;

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
UnDecorateSymbolName(
    autoboost::detail::winapi::LPCSTR_ DecoratedName,
    autoboost::detail::winapi::LPSTR_ UnDecoratedName,
    autoboost::detail::winapi::DWORD_ UndecoratedLength,
    autoboost::detail::winapi::DWORD_ Flags);

#if defined( AUTOBOOST_DETAIL_WINAPI_HAS_UNDECORATESYMBOLNAMEW )
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
UnDecorateSymbolNameW(
    autoboost::detail::winapi::LPCWSTR_ DecoratedName,
    autoboost::detail::winapi::LPWSTR_ UnDecoratedName,
    autoboost::detail::winapi::DWORD_ UndecoratedLength,
    autoboost::detail::winapi::DWORD_ Flags);
#endif

AUTOBOOST_SYMBOL_IMPORT API_VERSION* WINAPI
ImagehlpApiVersion(AUTOBOOST_DETAIL_WINAPI_VOID);

} // extern "C"
#endif

namespace autoboost {
namespace detail {
namespace winapi {

#if defined( AUTOBOOST_USE_WINDOWS_H )

const DWORD_ UNDNAME_COMPLETE_ = UNDNAME_COMPLETE;
const DWORD_ UNDNAME_NO_LEADING_UNDERSCORES_ = UNDNAME_NO_LEADING_UNDERSCORES;
const DWORD_ UNDNAME_NO_MS_KEYWORDS_ = UNDNAME_NO_MS_KEYWORDS;
const DWORD_ UNDNAME_NO_FUNCTION_RETURNS_ = UNDNAME_NO_FUNCTION_RETURNS;
const DWORD_ UNDNAME_NO_ALLOCATION_MODEL_ = UNDNAME_NO_ALLOCATION_MODEL;
const DWORD_ UNDNAME_NO_ALLOCATION_LANGUAGE_ = UNDNAME_NO_ALLOCATION_LANGUAGE;
const DWORD_ UNDNAME_NO_MS_THISTYPE_ = UNDNAME_NO_MS_THISTYPE;
const DWORD_ UNDNAME_NO_CV_THISTYPE_ = UNDNAME_NO_CV_THISTYPE;
const DWORD_ UNDNAME_NO_THISTYPE_ = UNDNAME_NO_THISTYPE;
const DWORD_ UNDNAME_NO_ACCESS_SPECIFIERS_ = UNDNAME_NO_ACCESS_SPECIFIERS;
const DWORD_ UNDNAME_NO_THROW_SIGNATURES_ = UNDNAME_NO_THROW_SIGNATURES;
const DWORD_ UNDNAME_NO_MEMBER_TYPE_ = UNDNAME_NO_MEMBER_TYPE;
const DWORD_ UNDNAME_NO_RETURN_UDT_MODEL_ = UNDNAME_NO_RETURN_UDT_MODEL;
const DWORD_ UNDNAME_32_BIT_DECODE_ = UNDNAME_32_BIT_DECODE;
const DWORD_ UNDNAME_NAME_ONLY_ = UNDNAME_NAME_ONLY;
const DWORD_ UNDNAME_NO_ARGUMENTS_ = UNDNAME_NO_ARGUMENTS;
const DWORD_ UNDNAME_NO_SPECIAL_SYMS_ = UNDNAME_NO_SPECIAL_SYMS;

#else // defined( AUTOBOOST_USE_WINDOWS_H )

const DWORD_ UNDNAME_COMPLETE_ = 0x00000000;
const DWORD_ UNDNAME_NO_LEADING_UNDERSCORES_ = 0x00000001;
const DWORD_ UNDNAME_NO_MS_KEYWORDS_ = 0x00000002;
const DWORD_ UNDNAME_NO_FUNCTION_RETURNS_ = 0x00000004;
const DWORD_ UNDNAME_NO_ALLOCATION_MODEL_ = 0x00000008;
const DWORD_ UNDNAME_NO_ALLOCATION_LANGUAGE_ = 0x00000010;
const DWORD_ UNDNAME_NO_MS_THISTYPE_ = 0x00000020;
const DWORD_ UNDNAME_NO_CV_THISTYPE_ = 0x00000040;
const DWORD_ UNDNAME_NO_THISTYPE_ = 0x00000060;
const DWORD_ UNDNAME_NO_ACCESS_SPECIFIERS_ = 0x00000080;
const DWORD_ UNDNAME_NO_THROW_SIGNATURES_ = 0x00000100;
const DWORD_ UNDNAME_NO_MEMBER_TYPE_ = 0x00000200;
const DWORD_ UNDNAME_NO_RETURN_UDT_MODEL_ = 0x00000400;
const DWORD_ UNDNAME_32_BIT_DECODE_ = 0x00000800;
const DWORD_ UNDNAME_NAME_ONLY_ = 0x00001000;
const DWORD_ UNDNAME_NO_ARGUMENTS_ = 0x00002000;
const DWORD_ UNDNAME_NO_SPECIAL_SYMS_ = 0x00004000;

#endif // defined( AUTOBOOST_USE_WINDOWS_H )

using ::UnDecorateSymbolName;
#if defined( AUTOBOOST_DETAIL_WINAPI_HAS_UNDECORATESYMBOLNAMEW )
using ::UnDecorateSymbolNameW;
#endif

typedef struct AUTOBOOST_DETAIL_WINAPI_MAY_ALIAS API_VERSION {
    USHORT_  MajorVersion;
    USHORT_  MinorVersion;
    USHORT_  Revision;
    USHORT_  Reserved;
} API_VERSION_, *LPAPI_VERSION_;

AUTOBOOST_FORCEINLINE LPAPI_VERSION_ ImagehlpApiVersion()
{
    return reinterpret_cast<LPAPI_VERSION_>(::ImagehlpApiVersion());
}

AUTOBOOST_FORCEINLINE DWORD_ undecorate_symbol_name(
    LPCSTR_ DecoratedName,
    LPSTR_ UnDecoratedName,
    DWORD_ UndecoratedLength,
    DWORD_ Flags)
{
    return ::UnDecorateSymbolName(
        DecoratedName,
        UnDecoratedName,
        UndecoratedLength,
        Flags);
}

#if defined( AUTOBOOST_DETAIL_WINAPI_HAS_UNDECORATESYMBOLNAMEW )

AUTOBOOST_FORCEINLINE DWORD_ undecorate_symbol_name(
    LPCWSTR_ DecoratedName,
    LPWSTR_ UnDecoratedName,
    DWORD_ UndecoratedLength,
    DWORD_ Flags)
{
    return ::UnDecorateSymbolNameW(
        DecoratedName,
        UnDecoratedName,
        UndecoratedLength,
        Flags);
}

#endif

}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_DBGHELP_HPP
