//  file_management.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2015 Andrey Semashev
//  Copyright 2016 Jorge Lodos

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_FILE_MANAGEMENT_HPP
#define AUTOBOOST_DETAIL_WINAPI_FILE_MANAGEMENT_HPP

#include <autoboost/detail/winapi/basic_types.hpp>
#include <autoboost/detail/winapi/limits.hpp>
#include <autoboost/detail/winapi/time.hpp>
#include <autoboost/detail/winapi/overlapped.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {

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

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
GetFileAttributesA(autoboost::detail::winapi::LPCSTR_ lpFileName);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
AreFileApisANSI(AUTOBOOST_DETAIL_WINAPI_VOID);
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

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
GetFileAttributesW(autoboost::detail::winapi::LPCWSTR_ lpFileName);

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
ReadFile(
    autoboost::detail::winapi::HANDLE_ hFile,
    autoboost::detail::winapi::LPVOID_ lpBuffer,
    autoboost::detail::winapi::DWORD_ nNumberOfBytesToRead,
    autoboost::detail::winapi::LPDWORD_ lpNumberOfBytesRead,
    ::_OVERLAPPED* lpOverlapped);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
WriteFile(
    autoboost::detail::winapi::HANDLE_ hFile,
    autoboost::detail::winapi::LPCVOID_ lpBuffer,
    autoboost::detail::winapi::DWORD_ nNumberOfBytesToWrite,
    autoboost::detail::winapi::LPDWORD_ lpNumberOfBytesWritten,
    ::_OVERLAPPED* lpOverlapped);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::DWORD_ WINAPI
SetFilePointer(
    autoboost::detail::winapi::HANDLE_ hFile,
    autoboost::detail::winapi::LONG_ lpDistanceToMove,
    autoboost::detail::winapi::PLONG_ lpDistanceToMoveHigh,
    autoboost::detail::winapi::DWORD_ dwMoveMethod);

struct _BY_HANDLE_FILE_INFORMATION;
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
GetFileInformationByHandle(
    autoboost::detail::winapi::HANDLE_ hFile,
    ::_BY_HANDLE_FILE_INFORMATION* lpFileInformation);
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {

#if defined( AUTOBOOST_USE_WINDOWS_H )

const DWORD_ INVALID_FILE_SIZE_ = INVALID_FILE_SIZE;
const DWORD_ INVALID_SET_FILE_POINTER_ = INVALID_SET_FILE_POINTER;
const DWORD_ INVALID_FILE_ATTRIBUTES_ = INVALID_FILE_ATTRIBUTES;

const DWORD_ FILE_ATTRIBUTE_READONLY_ = FILE_ATTRIBUTE_READONLY;
const DWORD_ FILE_ATTRIBUTE_HIDDEN_ = FILE_ATTRIBUTE_HIDDEN;
const DWORD_ FILE_ATTRIBUTE_SYSTEM_ = FILE_ATTRIBUTE_SYSTEM;
const DWORD_ FILE_ATTRIBUTE_DIRECTORY_ = FILE_ATTRIBUTE_DIRECTORY;
const DWORD_ FILE_ATTRIBUTE_ARCHIVE_ = FILE_ATTRIBUTE_ARCHIVE;
const DWORD_ FILE_ATTRIBUTE_DEVICE_ = FILE_ATTRIBUTE_DEVICE;
const DWORD_ FILE_ATTRIBUTE_NORMAL_ = FILE_ATTRIBUTE_NORMAL;
const DWORD_ FILE_ATTRIBUTE_TEMPORARY_ = FILE_ATTRIBUTE_TEMPORARY;
const DWORD_ FILE_ATTRIBUTE_SPARSE_FILE_ = FILE_ATTRIBUTE_SPARSE_FILE;
const DWORD_ FILE_ATTRIBUTE_REPARSE_POINT_ = FILE_ATTRIBUTE_REPARSE_POINT;
const DWORD_ FILE_ATTRIBUTE_COMPRESSED_ = FILE_ATTRIBUTE_COMPRESSED;
const DWORD_ FILE_ATTRIBUTE_OFFLINE_ = FILE_ATTRIBUTE_OFFLINE;
const DWORD_ FILE_ATTRIBUTE_NOT_CONTENT_INDEXED_ = FILE_ATTRIBUTE_NOT_CONTENT_INDEXED;
const DWORD_ FILE_ATTRIBUTE_ENCRYPTED_ = FILE_ATTRIBUTE_ENCRYPTED;

const DWORD_ CREATE_NEW_ = CREATE_NEW;
const DWORD_ CREATE_ALWAYS_ = CREATE_ALWAYS;
const DWORD_ OPEN_EXISTING_ = OPEN_EXISTING;
const DWORD_ OPEN_ALWAYS_ = OPEN_ALWAYS;
const DWORD_ TRUNCATE_EXISTING_ = TRUNCATE_EXISTING;

const DWORD_ FILE_SHARE_READ_ = FILE_SHARE_READ;
const DWORD_ FILE_SHARE_WRITE_ = FILE_SHARE_WRITE;
const DWORD_ FILE_SHARE_DELETE_ = FILE_SHARE_DELETE;

const DWORD_ FILE_BEGIN_ = FILE_BEGIN;
const DWORD_ FILE_CURRENT_ = FILE_CURRENT;
const DWORD_ FILE_END_ = FILE_END;

#else // defined( AUTOBOOST_USE_WINDOWS_H )

const DWORD_ INVALID_FILE_SIZE_ = ((DWORD_)0xFFFFFFFF);
const DWORD_ INVALID_SET_FILE_POINTER_ = ((DWORD_)-1);
const DWORD_ INVALID_FILE_ATTRIBUTES_ = ((DWORD_)-1);

const DWORD_ FILE_ATTRIBUTE_READONLY_ = 0x00000001;
const DWORD_ FILE_ATTRIBUTE_HIDDEN_ = 0x00000002;
const DWORD_ FILE_ATTRIBUTE_SYSTEM_ = 0x00000004;
const DWORD_ FILE_ATTRIBUTE_DIRECTORY_ = 0x00000010;
const DWORD_ FILE_ATTRIBUTE_ARCHIVE_ = 0x00000020;
const DWORD_ FILE_ATTRIBUTE_DEVICE_ = 0x00000040;
const DWORD_ FILE_ATTRIBUTE_NORMAL_ = 0x00000080;
const DWORD_ FILE_ATTRIBUTE_TEMPORARY_ = 0x00000100;
const DWORD_ FILE_ATTRIBUTE_SPARSE_FILE_ = 0x00000200;
const DWORD_ FILE_ATTRIBUTE_REPARSE_POINT_ = 0x00000400;
const DWORD_ FILE_ATTRIBUTE_COMPRESSED_ = 0x00000800;
const DWORD_ FILE_ATTRIBUTE_OFFLINE_ = 0x00001000;
const DWORD_ FILE_ATTRIBUTE_NOT_CONTENT_INDEXED_ = 0x00002000;
const DWORD_ FILE_ATTRIBUTE_ENCRYPTED_ = 0x00004000;

const DWORD_ CREATE_NEW_ = 1;
const DWORD_ CREATE_ALWAYS_ = 2;
const DWORD_ OPEN_EXISTING_ = 3;
const DWORD_ OPEN_ALWAYS_ = 4;
const DWORD_ TRUNCATE_EXISTING_ = 5;

const DWORD_ FILE_SHARE_READ_ = 0x00000001;
const DWORD_ FILE_SHARE_WRITE_ = 0x00000002;
const DWORD_ FILE_SHARE_DELETE_ = 0x00000004;

const DWORD_ FILE_BEGIN_ = 0;
const DWORD_ FILE_CURRENT_ = 1;
const DWORD_ FILE_END_ = 2;

#endif // defined( AUTOBOOST_USE_WINDOWS_H )

// This constant is not defined in Windows SDK up until 6.0A
const DWORD_ FILE_ATTRIBUTE_VIRTUAL_ = 0x00010000;

// These constants are not defined in Windows SDK up until 8.0 and MinGW/MinGW-w64 (as of 2016-02-14).
// They are documented to be supported only since Windows 8/Windows Server 2012
// but defined unconditionally.
const DWORD_ FILE_ATTRIBUTE_INTEGRITY_STREAM_ = 0x00008000;
const DWORD_ FILE_ATTRIBUTE_NO_SCRUB_DATA_ = 0x00020000;
// Undocumented
const DWORD_ FILE_ATTRIBUTE_EA_ = 0x00040000;

#if !defined( AUTOBOOST_NO_ANSI_APIS )
using ::DeleteFileA;
using ::MoveFileExA;
using ::GetFileAttributesA;
using ::AreFileApisANSI;
#endif

using ::DeleteFileW;
using ::MoveFileExW;
using ::GetFileAttributesW;

using ::FindClose;

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WINXP

using ::SetFileValidData;

#endif

using ::SetEndOfFile;
using ::LockFile;
using ::UnlockFile;
using ::SetFilePointer;


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
    CHAR_   cFileName[MAX_PATH_];
    CHAR_   cAlternateFileName[14];
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
    WCHAR_  cFileName[MAX_PATH_];
    WCHAR_  cAlternateFileName[14];
#ifdef _MAC
    DWORD_ dwFileType;
    DWORD_ dwCreatorType;
    WORD_  wFinderFlags;
#endif
} WIN32_FIND_DATAW_, *PWIN32_FIND_DATAW_, *LPWIN32_FIND_DATAW_;

typedef struct AUTOBOOST_DETAIL_WINAPI_MAY_ALIAS _BY_HANDLE_FILE_INFORMATION {
    DWORD_ dwFileAttributes;
    FILETIME_ ftCreationTime;
    FILETIME_ ftLastAccessTime;
    FILETIME_ ftLastWriteTime;
    DWORD_ dwVolumeSerialNumber;
    DWORD_ nFileSizeHigh;
    DWORD_ nFileSizeLow;
    DWORD_ nNumberOfLinks;
    DWORD_ nFileIndexHigh;
    DWORD_ nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION_, *PBY_HANDLE_FILE_INFORMATION_, *LPBY_HANDLE_FILE_INFORMATION_;

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

AUTOBOOST_FORCEINLINE BOOL_ ReadFile(
    HANDLE_ hFile,
    LPVOID_ lpBuffer,
    DWORD_ nNumberOfBytesToWrite,
    LPDWORD_ lpNumberOfBytesWritten,
    OVERLAPPED_* lpOverlapped)
{
    return ::ReadFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, reinterpret_cast< ::_OVERLAPPED* >(lpOverlapped));
}

AUTOBOOST_FORCEINLINE BOOL_ WriteFile(
    HANDLE_ hFile,
    LPCVOID_ lpBuffer,
    DWORD_ nNumberOfBytesToWrite,
    LPDWORD_ lpNumberOfBytesWritten,
    OVERLAPPED_* lpOverlapped)
{
    return ::WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, reinterpret_cast< ::_OVERLAPPED* >(lpOverlapped));
}

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

AUTOBOOST_FORCEINLINE DWORD_ get_file_attributes(LPCSTR_ lpFileName)
{
    return ::GetFileAttributesA(lpFileName);
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

AUTOBOOST_FORCEINLINE DWORD_ get_file_attributes(LPCWSTR_ lpFileName)
{
    return ::GetFileAttributesW(lpFileName);
}

AUTOBOOST_FORCEINLINE BOOL_ GetFileInformationByHandle(HANDLE_ h, BY_HANDLE_FILE_INFORMATION_* info)
{
    return ::GetFileInformationByHandle(h, reinterpret_cast< ::_BY_HANDLE_FILE_INFORMATION* >(info));
}

}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_FILE_MANAGEMENT_HPP
