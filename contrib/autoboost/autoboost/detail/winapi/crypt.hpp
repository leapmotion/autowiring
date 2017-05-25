//  crypt.hpp  --------------------------------------------------------------//

//  Copyright 2014 Antony Polukhin
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_CRYPT_HPP
#define AUTOBOOST_DETAIL_WINAPI_CRYPT_HPP

#include <autoboost/detail/winapi/basic_types.hpp>
#include <autoboost/detail/winapi/detail/cast_ptr.hpp>
#if defined( AUTOBOOST_USE_WINDOWS_H )
// This header is not always included as part of windows.h
#include <wincrypt.h>
#endif

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
namespace autoboost { namespace detail { namespace winapi {
typedef ULONG_PTR_ HCRYPTPROV_;
}}}

// Some versions of MinGW (including the latest ones) contain buggy declarations of CryptEnumProvidersA and CryptEnumProvidersW.
// We cannot detect those broken versions, and we can't include the system header because it's incomplete.
// So below we duplicate the broken declarations here and work around the problem with cast_ptr. These declarations
// will have to be removed when MinGW is fixed.

extern "C" {
#if !defined( AUTOBOOST_NO_ANSI_APIS )
#if !defined( AUTOBOOST_WINAPI_IS_MINGW ) || !defined( UNICODE )
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
CryptEnumProvidersA(
    autoboost::detail::winapi::DWORD_ dwIndex,
    autoboost::detail::winapi::DWORD_ *pdwReserved,
    autoboost::detail::winapi::DWORD_ dwFlags,
    autoboost::detail::winapi::DWORD_ *pdwProvType,
    autoboost::detail::winapi::LPSTR_ szProvName,
    autoboost::detail::winapi::DWORD_ *pcbProvName);
#else
// Broken declaration in MinGW
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
CryptEnumProvidersA(
    autoboost::detail::winapi::DWORD_ dwIndex,
    autoboost::detail::winapi::DWORD_ *pdwReserved,
    autoboost::detail::winapi::DWORD_ dwFlags,
    autoboost::detail::winapi::DWORD_ *pdwProvType,
    autoboost::detail::winapi::LPWSTR_ szProvName,
    autoboost::detail::winapi::DWORD_ *pcbProvName);
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
CryptAcquireContextA(
    autoboost::detail::winapi::HCRYPTPROV_ *phProv,
    autoboost::detail::winapi::LPCSTR_ pszContainer,
    autoboost::detail::winapi::LPCSTR_ pszProvider,
    autoboost::detail::winapi::DWORD_ dwProvType,
    autoboost::detail::winapi::DWORD_ dwFlags);
#endif // !defined( AUTOBOOST_NO_ANSI_APIS )

#if !defined( AUTOBOOST_WINAPI_IS_MINGW ) || defined( UNICODE )
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
CryptEnumProvidersW(
    autoboost::detail::winapi::DWORD_ dwIndex,
    autoboost::detail::winapi::DWORD_ *pdwReserved,
    autoboost::detail::winapi::DWORD_ dwFlags,
    autoboost::detail::winapi::DWORD_ *pdwProvType,
    autoboost::detail::winapi::LPWSTR_ szProvName,
    autoboost::detail::winapi::DWORD_ *pcbProvName);
#else
// Broken declaration in MinGW
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
CryptEnumProvidersW(
    autoboost::detail::winapi::DWORD_ dwIndex,
    autoboost::detail::winapi::DWORD_ *pdwReserved,
    autoboost::detail::winapi::DWORD_ dwFlags,
    autoboost::detail::winapi::DWORD_ *pdwProvType,
    autoboost::detail::winapi::LPSTR_ szProvName,
    autoboost::detail::winapi::DWORD_ *pcbProvName);
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
CryptAcquireContextW(
    autoboost::detail::winapi::HCRYPTPROV_ *phProv,
    autoboost::detail::winapi::LPCWSTR_ szContainer,
    autoboost::detail::winapi::LPCWSTR_ szProvider,
    autoboost::detail::winapi::DWORD_ dwProvType,
    autoboost::detail::winapi::DWORD_ dwFlags);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
CryptGenRandom(
    autoboost::detail::winapi::HCRYPTPROV_ hProv,
    autoboost::detail::winapi::DWORD_ dwLen,
    autoboost::detail::winapi::BYTE_ *pbBuffer);

#if defined(_MSC_VER) && (_MSC_VER+0) >= 1500 &&\
    (\
        (defined(NTDDI_VERSION) && (NTDDI_VERSION+0) < AUTOBOOST_DETAIL_WINAPI_MAKE_NTDDI_VERSION(AUTOBOOST_WINAPI_VERSION_WINXP)) ||\
        (!defined(NTDDI_VERSION) && AUTOBOOST_USE_WINAPI_VERSION < AUTOBOOST_WINAPI_VERSION_WINXP)\
    )
// Standalone MS Windows SDK 6.0A and later provide a different declaration of CryptReleaseContext for Windows 2000 and older.
// This is not the case for (a) MinGW and MinGW-w64 and (b) MSVC 7.1 and 8, which are shipped with their own Windows SDK.
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
CryptReleaseContext(
    autoboost::detail::winapi::HCRYPTPROV_ hProv,
    autoboost::detail::winapi::ULONG_PTR_ dwFlags);
#else
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
CryptReleaseContext(
    autoboost::detail::winapi::HCRYPTPROV_ hProv,
    autoboost::detail::winapi::DWORD_ dwFlags);
#endif
}
#endif // !defined( AUTOBOOST_USE_WINDOWS_H )

namespace autoboost {
namespace detail {
namespace winapi {

#if defined( AUTOBOOST_USE_WINDOWS_H )

typedef ::HCRYPTPROV HCRYPTPROV_;

const DWORD_ PROV_RSA_FULL_         = PROV_RSA_FULL;

const DWORD_ CRYPT_VERIFYCONTEXT_   = CRYPT_VERIFYCONTEXT;
const DWORD_ CRYPT_NEWKEYSET_       = CRYPT_NEWKEYSET;
const DWORD_ CRYPT_DELETEKEYSET_    = CRYPT_DELETEKEYSET;
const DWORD_ CRYPT_MACHINE_KEYSET_  = CRYPT_MACHINE_KEYSET;
const DWORD_ CRYPT_SILENT_          = CRYPT_SILENT;

#else

const DWORD_ PROV_RSA_FULL_         = 1;

const DWORD_ CRYPT_VERIFYCONTEXT_   = 0xF0000000;
const DWORD_ CRYPT_NEWKEYSET_       = 8;
const DWORD_ CRYPT_DELETEKEYSET_    = 16;
const DWORD_ CRYPT_MACHINE_KEYSET_  = 32;
const DWORD_ CRYPT_SILENT_          = 64;

#endif

#if !defined( AUTOBOOST_NO_ANSI_APIS )
using ::CryptAcquireContextA;
#endif
using ::CryptAcquireContextW;
using ::CryptGenRandom;

#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_FORCEINLINE BOOL_ CryptEnumProvidersA(
    DWORD_ dwIndex,
    DWORD_ *pdwReserved,
    DWORD_ dwFlags,
    DWORD_ *pdwProvType,
    LPSTR_ szProvName,
    DWORD_ *pcbProvName)
{
    return ::CryptEnumProvidersA(dwIndex, pdwReserved, dwFlags, pdwProvType, winapi::detail::cast_ptr(szProvName), pcbProvName);
}

AUTOBOOST_FORCEINLINE BOOL_ crypt_enum_providers(
    DWORD_ dwIndex,
    DWORD_ *pdwReserved,
    DWORD_ dwFlags,
    DWORD_ *pdwProvType,
    LPSTR_ szProvName,
    DWORD_ *pcbProvName)
{
    return ::CryptEnumProvidersA(dwIndex, pdwReserved, dwFlags, pdwProvType, winapi::detail::cast_ptr(szProvName), pcbProvName);
}

AUTOBOOST_FORCEINLINE BOOL_ crypt_acquire_context(
    HCRYPTPROV_ *phProv,
    LPCSTR_ pszContainer,
    LPCSTR_ pszProvider,
    DWORD_ dwProvType,
    DWORD_ dwFlags)
{
    return ::CryptAcquireContextA(phProv, pszContainer, pszProvider, dwProvType, dwFlags);
}
#endif

AUTOBOOST_FORCEINLINE BOOL_ CryptEnumProvidersW(
    DWORD_ dwIndex,
    DWORD_ *pdwReserved,
    DWORD_ dwFlags,
    DWORD_ *pdwProvType,
    LPWSTR_ szProvName,
    DWORD_ *pcbProvName)
{
    return ::CryptEnumProvidersW(dwIndex, pdwReserved, dwFlags, pdwProvType, winapi::detail::cast_ptr(szProvName), pcbProvName);
}

AUTOBOOST_FORCEINLINE BOOL_ crypt_enum_providers(
    DWORD_ dwIndex,
    DWORD_ *pdwReserved,
    DWORD_ dwFlags,
    DWORD_ *pdwProvType,
    LPWSTR_ szProvName,
    DWORD_ *pcbProvName)
{
    return ::CryptEnumProvidersW(dwIndex, pdwReserved, dwFlags, pdwProvType, winapi::detail::cast_ptr(szProvName), pcbProvName);
}

AUTOBOOST_FORCEINLINE BOOL_ crypt_acquire_context(
    HCRYPTPROV_ *phProv,
    LPCWSTR_ szContainer,
    LPCWSTR_ szProvider,
    DWORD_ dwProvType,
    DWORD_ dwFlags)
{
    return ::CryptAcquireContextW(phProv, szContainer, szProvider, dwProvType, dwFlags);
}

AUTOBOOST_FORCEINLINE BOOL_ CryptReleaseContext(HCRYPTPROV_ hProv, DWORD_ dwFlags)
{
    return ::CryptReleaseContext(hProv, dwFlags);
}

}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_CRYPT_HPP
