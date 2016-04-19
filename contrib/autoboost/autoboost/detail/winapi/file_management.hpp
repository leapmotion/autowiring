//  file_management.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_FILE_MANAGEMENT_HPP
#define AUTOBOOST_DETAIL_WINAPI_FILE_MANAGEMENT_HPP

#include <autoboost/detail/winapi/basic_types.hpp>
#include <autoboost/detail/winapi/time.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
struct _OVERLAPPED;

#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
CreateFileA(
    autoboost::detail::winapi::LPCSTR_ lpFileName,
    autoboost::detail::winapi::DWORD_ dwDesiredAccess,
    autoboost::detail::winapi::DWORD_ dwShareMode,
    ::_SECURITY_ATTRIBUTES* lpSecurityAttributes,
    autoboost::detail::winapi::DWORD_ dwCreationDisposition,
    autoboost::detail::winapi::DWORD_ dwFlagsAndAttributes,
    autoboost::detail::winapi::HANDLE_ hTemplateFile);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
DeleteFileA(autoboost::detail::winapi::LPCSTR_ lpFileName);

struct _WIN32_FIND_DATAA;
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
FindFirstFileA(autoboost::detail::winapi::LPCSTR_ lpFileName, ::_WIN32_FIND_DATAA* lpFindFileData);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
FindNextFileA(autoboost::detail::winapi::HANDLE_ hFindFile, ::_WIN32_FIND_DATAA* lpFindFileData);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
MoveFileExA(
    autoboost::detail::winapi::LPCSTR_ lpExistingFileName,
    autoboost::detail::winapi::LPCSTR_ lpNewFileName,
    autoboost::detail::winapi::DWORD_ dwFlags);
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
CreateFileW(
    autoboost::detail::winapi::LPCWSTR_ lpFileName,
    autoboost::detail::winapi::DWORD_ dwDesiredAccess,
    autoboost::detail::winapi::DWORD_ dwShareMode,
    ::_SECURITY_ATTRIBUTES* lpSecurityAttributes,
    autoboost::detail::winapi::DWORD_ dwCreationDisposition,
    autoboost::detail::winapi::DWORD_ dwFlagsAndAttributes,
    autoboost::detail::winapi::HANDLE_ hTemplateFile);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
DeleteFileW(autoboost::detail::winapi::LPCWSTR_ lpFileName);

struct _WIN32_FIND_DATAW;
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI
FindFirstFileW(autoboost::detail::winapi::LPCWSTR_ lpFileName, ::_WIN32_FIND_DATAW* lpFindFileData);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
FindNextFileW(autoboost::detail::winapi::HANDLE_ hFindFile, ::_WIN32_FIND_DATAW* lpFindFileData);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
MoveFileExW(
    autoboost::detail::winapi::LPCWSTR_ lpExistingFileName,
    autoboost::detail::winapi::LPCWSTR_ lpNewFileName,
    autoboost::detail::winapi::DWORD_ dwFlags);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
FindClose(autoboost::detail::winapi::HANDLE_ hFindFile);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
GetFileSizeEx(autoboost::detail::winapi::HANDLE_ hFile, ::_LARGE_INTEGER* lpFileSize);

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WINXP

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
SetFileValidData(autoboost::detail::winapi::HANDLE_ hFile, autoboost::detail::winapi::LONGLONG_ ValidDataLength);

#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
SetEndOfFile(autoboost::detail::winapi::HANDLE_ hFile);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
LockFile(
    autoboost::detail::winapi::HANDLE_ hFile,
    autoboost::detail::winapi::DWORD_ dwFileOffsetLow,
    autoboost::detail::winapi::DWORD_ dwFileOffsetHigh,
    autoboost::detail::winapi::DWORD_ nNumberOfBytesToLockLow,
    autoboost::detail::winapi::DWORD_ nNumberOfBytesToLockHigh);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
UnlockFile(
    autoboost::detail::winapi::HANDLE_ hFile,
    autoboost::detail::winapi::DWORD_ dwFileOffsetLow,
    autoboost::detail::winapi::DWORD_ dwFileOffsetHigh,
    autoboost::detail::winapi::DWORD_ nNumberOfBytesToUnlockLow,
    autoboost::detail::winapi::DWORD_ nNumberOfBytesToUnlockHigh);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
LockFileEx(
    autoboost::detail::winapi::HANDLE_ hFile,
    autoboost::detail::winapi::DWORD_ dwFlags,
    autoboost::detail::winapi::DWORD_ dwReserved,
    autoboost::detail::winapi::DWORD_ nNumberOfBytesToLockLow,
    autoboost::detail::winapi::DWORD_ nNumberOfBytesToLockHigh,
    ::_OVERLAPPED* lpOverlapped);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
UnlockFileEx(
    autoboost::detail::winapi::HANDLE_ hFile,
    autoboost::detail::winapi::DWORD_ dwReserved,
    autoboost::detail::winapi::DWORD_ nNumberOfBytesToUnlockLow,
    autoboost::detail::winapi::DWORD_ nNumberOfBytesToUnlockHigh,
    ::_OVERLAPPED* lpOverlapped);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
WriteFile(
    autoboost::detail::winapi::HANDLE_ hFile,
    autoboost::detail::winapi::LPCVOID_ lpBuffer,
    autoboost::detail::winapi::DWORD_ nNumberOfBytesToWrite,
    autoboost::detail::winapi::LPDWORD_ lpNumberOfBytesWritten,
    ::_OVERLAPPED* lpOverlapped);
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {

#if !defined( AUTOBOOST_NO_ANSI_APIS )
using ::DeleteFileA;
using ::MoveFileExA;
#endif

using ::DeleteFileW;
using ::MoveFileExW;

using ::FindClose;

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WINXP

using ::SetFileValidData;

#endif

using ::SetEndOfFile;
using ::LockFile;
using ::UnlockFile;

typedef struct AUTOBOOST_DETAIL_WINAPI_MAY_ALIAS _OVERLAPPED {
    ULONG_PTR_ Internal;
    ULONG_PTR_ InternalHigh;
    union {
        struct {
            DWORD_ Offset;
            DWORD_ OffsetHigh;
        };
        PVOID_  Pointer;
    };
    HANDLE_    hEvent;
} OVERLAPPED_, *LPOVERLAPPED_;

#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_FORCEINLINE HANDLE_ CreateFileA(
    LPCSTR_ lpFileName,
    DWORD_ dwDesiredAccess,
    DWORD_ dwShareMode,
    SECURITY_ATTRIBUTES_* lpSecurityAttributes,
    DWORD_ dwCreationDisposition,
    DWORD_ dwFlagsAndAttributes,
    HANDLE_ hTemplateFile)
{
    return ::CreateFileA(
        lpFileName,
        dwDesiredAccess,
        dwShareMode,
        reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpSecurityAttributes),
        dwCreationDisposition,
        dwFlagsAndAttributes,
        hTemplateFile);
}

typedef struct AUTOBOOST_DETAIL_WINAPI_MAY_ALIAS _WIN32_FIND_DATAA {
    DWORD_ dwFileAttributes;
    FILETIME_ ftCreationTime;
    FILETIME_ ftLastAccessTime;
    FILETIME_ ftLastWriteTime;
    DWORD_ nFileSizeHigh;
    DWORD_ nFileSizeLow;
    DWORD_ dwReserved0;
    DWORD_ dwReserved1;
    CHAR_   cFileName[ 260 ]; // MAX_PATH
    CHAR_   cAlternateFileName[ 14 ];
#ifdef _MAC
    DWORD_ dwFileType;
    DWORD_ dwCreatorType;
    WORD_  wFinderFlags;
#endif
} WIN32_FIND_DATAA_, *PWIN32_FIND_DATAA_, *LPWIN32_FIND_DATAA_;

AUTOBOOST_FORCEINLINE HANDLE_ FindFirstFileA(LPCSTR_ lpFileName, WIN32_FIND_DATAA_* lpFindFileData)
{
    return ::FindFirstFileA(lpFileName, reinterpret_cast< ::_WIN32_FIND_DATAA* >(lpFindFileData));
}

AUTOBOOST_FORCEINLINE BOOL_ FindNextFileA(HANDLE_ hFindFile, WIN32_FIND_DATAA_* lpFindFileData)
{
    return ::FindNextFileA(hFindFile, reinterpret_cast< ::_WIN32_FIND_DATAA* >(lpFindFileData));
}
#endif

AUTOBOOST_FORCEINLINE HANDLE_ CreateFileW(
    LPCWSTR_ lpFileName,
    DWORD_ dwDesiredAccess,
    DWORD_ dwShareMode,
    SECURITY_ATTRIBUTES_* lpSecurityAttributes,
    DWORD_ dwCreationDisposition,
    DWORD_ dwFlagsAndAttributes,
    HANDLE_ hTemplateFile)
{
    return ::CreateFileW(
        lpFileName,
        dwDesiredAccess,
        dwShareMode,
        reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpSecurityAttributes),
        dwCreationDisposition,
        dwFlagsAndAttributes,
        hTemplateFile);
}

typedef struct AUTOBOOST_DETAIL_WINAPI_MAY_ALIAS _WIN32_FIND_DATAW {
    DWORD_ dwFileAttributes;
    FILETIME_ ftCreationTime;
    FILETIME_ ftLastAccessTime;
    FILETIME_ ftLastWriteTime;
    DWORD_ nFileSizeHigh;
    DWORD_ nFileSizeLow;
    DWORD_ dwReserved0;
    DWORD_ dwReserved1;
    WCHAR_  cFileName[ 260 ]; // MAX_PATH
    WCHAR_  cAlternateFileName[ 14 ];
#ifdef _MAC
    DWORD_ dwFileType;
    DWORD_ dwCreatorType;
    WORD_  wFinderFlags;
#endif
} WIN32_FIND_DATAW_, *PWIN32_FIND_DATAW_, *LPWIN32_FIND_DATAW_;

AUTOBOOST_FORCEINLINE HANDLE_ FindFirstFileW(LPCWSTR_ lpFileName, WIN32_FIND_DATAW_* lpFindFileData)
{
    return ::FindFirstFileW(lpFileName, reinterpret_cast< ::_WIN32_FIND_DATAW* >(lpFindFileData));
}

AUTOBOOST_FORCEINLINE BOOL_ FindNextFileW(HANDLE_ hFindFile, WIN32_FIND_DATAW_* lpFindFileData)
{
    return ::FindNextFileW(hFindFile, reinterpret_cast< ::_WIN32_FIND_DATAW* >(lpFindFileData));
}

AUTOBOOST_FORCEINLINE BOOL_ GetFileSizeEx(HANDLE_ hFile, LARGE_INTEGER_* lpFileSize)
{
    return ::GetFileSizeEx(hFile, reinterpret_cast< ::_LARGE_INTEGER* >(lpFileSize));
}

AUTOBOOST_FORCEINLINE BOOL_ LockFileEx(
    HANDLE_ hFile,
    DWORD_ dwFlags,
    DWORD_ dwReserved,
    DWORD_ nNumberOfBytesToLockLow,
    DWORD_ nNumberOfBytesToLockHigh,
    OVERLAPPED_* lpOverlapped)
{
    return ::LockFileEx(hFile, dwFlags, dwReserved, nNumberOfBytesToLockLow, nNumberOfBytesToLockHigh, reinterpret_cast< ::_OVERLAPPED* >(lpOverlapped));
}

AUTOBOOST_FORCEINLINE BOOL_ UnlockFileEx(
    HANDLE_ hFile,
    DWORD_ dwReserved,
    DWORD_ nNumberOfBytesToUnlockLow,
    DWORD_ nNumberOfBytesToUnlockHigh,
    OVERLAPPED_* lpOverlapped)
{
    return ::UnlockFileEx(hFile, dwReserved, nNumberOfBytesToUnlockLow, nNumberOfBytesToUnlockHigh, reinterpret_cast< ::_OVERLAPPED* >(lpOverlapped));
}

AUTOBOOST_FORCEINLINE BOOL_ WriteFile(
    HANDLE_ hFile,
    LPCVOID_ lpBuffer,
    DWORD_ nNumberOfBytesToWrite,
    LPDWORD_ lpNumberOfBytesWritten,
    OVERLAPPED_* lpOverlapped)
{
    return ::WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, reinterpret_cast< ::_OVERLAPPED* >(lpOverlapped));
};


#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_FORCEINLINE HANDLE_ create_file(
    LPCSTR_ lpFileName,
    DWORD_ dwDesiredAccess,
    DWORD_ dwShareMode,
    SECURITY_ATTRIBUTES_* lpSecurityAttributes,
    DWORD_ dwCreationDisposition,
    DWORD_ dwFlagsAndAttributes,
    HANDLE_ hTemplateFile)
{
    return ::CreateFileA(
        lpFileName,
        dwDesiredAccess,
        dwShareMode,
        reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpSecurityAttributes),
        dwCreationDisposition,
        dwFlagsAndAttributes,
        hTemplateFile);
}

AUTOBOOST_FORCEINLINE BOOL_ delete_file(LPCSTR_ lpFileName)
{
    return ::DeleteFileA(lpFileName);
}

AUTOBOOST_FORCEINLINE HANDLE_ find_first_file(LPCSTR_ lpFileName, WIN32_FIND_DATAA_* lpFindFileData)
{
    return ::FindFirstFileA(lpFileName, reinterpret_cast< ::_WIN32_FIND_DATAA* >(lpFindFileData));
}

AUTOBOOST_FORCEINLINE BOOL_ find_next_file(HANDLE_ hFindFile, WIN32_FIND_DATAA_* lpFindFileData)
{
    return ::FindNextFileA(hFindFile, reinterpret_cast< ::_WIN32_FIND_DATAA* >(lpFindFileData));
}

AUTOBOOST_FORCEINLINE BOOL_ move_file(LPCSTR_ lpExistingFileName, LPCSTR_ lpNewFileName, DWORD_ dwFlags)
{
    return ::MoveFileExA(lpExistingFileName, lpNewFileName, dwFlags);
}
#endif

AUTOBOOST_FORCEINLINE HANDLE_ create_file(
    LPCWSTR_ lpFileName,
    DWORD_ dwDesiredAccess,
    DWORD_ dwShareMode,
    SECURITY_ATTRIBUTES_* lpSecurityAttributes,
    DWORD_ dwCreationDisposition,
    DWORD_ dwFlagsAndAttributes,
    HANDLE_ hTemplateFile)
{
    return ::CreateFileW(
        lpFileName,
        dwDesiredAccess,
        dwShareMode,
        reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpSecurityAttributes),
        dwCreationDisposition,
        dwFlagsAndAttributes,
        hTemplateFile);
}

AUTOBOOST_FORCEINLINE BOOL_ delete_file(LPCWSTR_ lpFileName)
{
    return ::DeleteFileW(lpFileName);
}

AUTOBOOST_FORCEINLINE HANDLE_ find_first_file(LPCWSTR_ lpFileName, WIN32_FIND_DATAW_* lpFindFileData)
{
    return ::FindFirstFileW(lpFileName, reinterpret_cast< ::_WIN32_FIND_DATAW* >(lpFindFileData));
}

AUTOBOOST_FORCEINLINE BOOL_ find_next_file(HANDLE_ hFindFile, WIN32_FIND_DATAW_* lpFindFileData)
{
    return ::FindNextFileW(hFindFile, reinterpret_cast< ::_WIN32_FIND_DATAW* >(lpFindFileData));
}

AUTOBOOST_FORCEINLINE BOOL_ move_file(LPCWSTR_ lpExistingFileName, LPCWSTR_ lpNewFileName, DWORD_ dwFlags)
{
    return ::MoveFileExW(lpExistingFileName, lpNewFileName, dwFlags);
}

}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_FILE_MANAGEMENT_HPP
