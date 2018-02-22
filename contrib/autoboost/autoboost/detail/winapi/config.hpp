//  config.hpp  --------------------------------------------------------------//

//  Copyright 2013 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_CONFIG_HPP_INCLUDED_
#define AUTOBOOST_DETAIL_WINAPI_CONFIG_HPP_INCLUDED_

#if defined __MINGW32__
#include <_mingw.h>
#endif

// AUTOBOOST_WINAPI_IS_MINGW indicates that the target Windows SDK is provided by MinGW (http://mingw.org/).
// AUTOBOOST_WINAPI_IS_MINGW_W64 indicates that the target Windows SDK is provided by MinGW-w64 (http://mingw-w64.org).
#if defined __MINGW32__
#if defined __MINGW64_VERSION_MAJOR
#define AUTOBOOST_WINAPI_IS_MINGW_W64
#else
#define AUTOBOOST_WINAPI_IS_MINGW
#endif
#endif

// These constants reflect _WIN32_WINNT_* macros from sdkddkver.h
// See also: http://msdn.microsoft.com/en-us/library/windows/desktop/aa383745%28v=vs.85%29.aspx#setting_winver_or__win32_winnt
#define AUTOBOOST_WINAPI_VERSION_NT4 0x0400
#define AUTOBOOST_WINAPI_VERSION_WIN2K 0x0500
#define AUTOBOOST_WINAPI_VERSION_WINXP 0x0501
#define AUTOBOOST_WINAPI_VERSION_WS03 0x0502
#define AUTOBOOST_WINAPI_VERSION_WIN6 0x0600
#define AUTOBOOST_WINAPI_VERSION_VISTA 0x0600
#define AUTOBOOST_WINAPI_VERSION_WS08 0x0600
#define AUTOBOOST_WINAPI_VERSION_LONGHORN 0x0600
#define AUTOBOOST_WINAPI_VERSION_WIN7 0x0601
#define AUTOBOOST_WINAPI_VERSION_WIN8 0x0602
#define AUTOBOOST_WINAPI_VERSION_WINBLUE 0x0603
#define AUTOBOOST_WINAPI_VERSION_WINTHRESHOLD 0x0A00
#define AUTOBOOST_WINAPI_VERSION_WIN10 0x0A00

#if !defined(AUTOBOOST_USE_WINAPI_VERSION)
#if defined(_WIN32_WINNT)
#define AUTOBOOST_USE_WINAPI_VERSION _WIN32_WINNT
#elif defined(WINVER)
#define AUTOBOOST_USE_WINAPI_VERSION WINVER
#else
// By default use Windows Vista API on compilers that support it and XP on the others
#if (defined(_MSC_VER) && _MSC_VER < 1500) || defined(AUTOBOOST_WINAPI_IS_MINGW)
#define AUTOBOOST_USE_WINAPI_VERSION AUTOBOOST_WINAPI_VERSION_WINXP
#else
#define AUTOBOOST_USE_WINAPI_VERSION AUTOBOOST_WINAPI_VERSION_WIN6
#endif
#endif
#endif

#define AUTOBOOST_DETAIL_WINAPI_MAKE_NTDDI_VERSION2(x) x##0000
#define AUTOBOOST_DETAIL_WINAPI_MAKE_NTDDI_VERSION(x) AUTOBOOST_DETAIL_WINAPI_MAKE_NTDDI_VERSION2(x)

#if defined(AUTOBOOST_USE_WINDOWS_H) || defined(AUTOBOOST_WINAPI_DEFINE_VERSION_MACROS)
// We have to define the version macros so that windows.h provides the necessary symbols
#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT AUTOBOOST_USE_WINAPI_VERSION
#endif
#if !defined(WINVER)
#define WINVER AUTOBOOST_USE_WINAPI_VERSION
#endif
#if !defined(NTDDI_VERSION)
#define NTDDI_VERSION AUTOBOOST_DETAIL_WINAPI_MAKE_NTDDI_VERSION(AUTOBOOST_USE_WINAPI_VERSION)
#endif
#endif

#include <autoboost/config.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#endif // AUTOBOOST_DETAIL_WINAPI_CONFIG_HPP_INCLUDED_
