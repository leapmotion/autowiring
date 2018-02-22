//  pipes.hpp  --------------------------------------------------------------//

//  Copyright 2016 Klemens D. Morgenstern
//  Copyright 2016 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#ifndef AUTOBOOST_DETAIL_WINAPI_PIPES_HPP_
#define AUTOBOOST_DETAIL_WINAPI_PIPES_HPP_

#include <autoboost/detail/winapi/basic_types.hpp>
#include <autoboost/detail/winapi/config.hpp>
#include <autoboost/detail/winapi/overlapped.hpp>
#include <autoboost/predef/platform.h>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if AUTOBOOST_PLAT_WINDOWS_DESKTOP

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI ImpersonateNamedPipeClient(
    autoboost::detail::winapi::HANDLE_ hNamedPipe);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI CreatePipe(
    autoboost::detail::winapi::PHANDLE_ hReadPipe,
    autoboost::detail::winapi::PHANDLE_ hWritePipe,
    _SECURITY_ATTRIBUTES* lpPipeAttributes,
    autoboost::detail::winapi::DWORD_ nSize);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI ConnectNamedPipe(
    autoboost::detail::winapi::HANDLE_ hNamedPipe,
    _OVERLAPPED* lpOverlapped);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI DisconnectNamedPipe(
    autoboost::detail::winapi::HANDLE_ hNamedPipe);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI SetNamedPipeHandleState(
    autoboost::detail::winapi::HANDLE_ hNamedPipe,
    autoboost::detail::winapi::LPDWORD_ lpMode,
    autoboost::detail::winapi::LPDWORD_ lpMaxCollectionCount,
    autoboost::detail::winapi::LPDWORD_ lpCollectDataTimeout);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI PeekNamedPipe(
    autoboost::detail::winapi::HANDLE_ hNamedPipe,
    autoboost::detail::winapi::LPVOID_ lpBuffer,
    autoboost::detail::winapi::DWORD_ nBufferSize,
    autoboost::detail::winapi::LPDWORD_ lpBytesRead,
    autoboost::detail::winapi::LPDWORD_ lpTotalBytesAvail,
    autoboost::detail::winapi::LPDWORD_ lpBytesLeftThisMessage);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI TransactNamedPipe(
    autoboost::detail::winapi::HANDLE_ hNamedPipe,
    autoboost::detail::winapi::LPVOID_ lpInBuffer,
    autoboost::detail::winapi::DWORD_ nInBufferSize,
    autoboost::detail::winapi::LPVOID_ lpOutBuffer,
    autoboost::detail::winapi::DWORD_ nOutBufferSize,
    autoboost::detail::winapi::LPDWORD_ lpBytesRead,
    _OVERLAPPED* lpOverlapped);

#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI CreateNamedPipeA(
    autoboost::detail::winapi::LPCSTR_ lpName,
    autoboost::detail::winapi::DWORD_ dwOpenMode,
    autoboost::detail::winapi::DWORD_ dwPipeMode,
    autoboost::detail::winapi::DWORD_ nMaxInstances,
    autoboost::detail::winapi::DWORD_ nOutBufferSize,
    autoboost::detail::winapi::DWORD_ nInBufferSize,
    autoboost::detail::winapi::DWORD_ nDefaultTimeOut,
    _SECURITY_ATTRIBUTES *lpSecurityAttributes);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI WaitNamedPipeA(
    autoboost::detail::winapi::LPCSTR_ lpNamedPipeName,
    autoboost::detail::winapi::DWORD_ nTimeOut);
#endif // !defined( AUTOBOOST_NO_ANSI_APIS )

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI CreateNamedPipeW(
    autoboost::detail::winapi::LPCWSTR_ lpName,
    autoboost::detail::winapi::DWORD_ dwOpenMode,
    autoboost::detail::winapi::DWORD_ dwPipeMode,
    autoboost::detail::winapi::DWORD_ nMaxInstances,
    autoboost::detail::winapi::DWORD_ nOutBufferSize,
    autoboost::detail::winapi::DWORD_ nInBufferSize,
    autoboost::detail::winapi::DWORD_ nDefaultTimeOut,
    _SECURITY_ATTRIBUTES* lpSecurityAttributes);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI WaitNamedPipeW(
    autoboost::detail::winapi::LPCWSTR_ lpNamedPipeName,
    autoboost::detail::winapi::DWORD_ nTimeOut);

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI GetNamedPipeClientComputerNameA(
    autoboost::detail::winapi::HANDLE_ Pipe,
    autoboost::detail::winapi::LPSTR_ ClientComputerName,
    autoboost::detail::winapi::ULONG_ ClientComputerNameLength);
#endif // !defined( AUTOBOOST_NO_ANSI_APIS )

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI GetNamedPipeClientComputerNameW(
    autoboost::detail::winapi::HANDLE_ Pipe,
    autoboost::detail::winapi::LPWSTR_ ClientComputerName,
    autoboost::detail::winapi::ULONG_ ClientComputerNameLength);
#endif

} // extern "C"
#endif // !defined( AUTOBOOST_USE_WINDOWS_H )

namespace autoboost {
namespace detail {
namespace winapi {

#if defined( AUTOBOOST_USE_WINDOWS_H )

const DWORD_ PIPE_ACCESS_DUPLEX_ = PIPE_ACCESS_DUPLEX;
const DWORD_ PIPE_ACCESS_INBOUND_ = PIPE_ACCESS_INBOUND;
const DWORD_ PIPE_ACCESS_OUTBOUND_ = PIPE_ACCESS_OUTBOUND;

const DWORD_ PIPE_TYPE_BYTE_ = PIPE_TYPE_BYTE;
const DWORD_ PIPE_TYPE_MESSAGE_ = PIPE_TYPE_MESSAGE;

const DWORD_ PIPE_READMODE_BYTE_ = PIPE_READMODE_BYTE;
const DWORD_ PIPE_READMODE_MESSAGE_ = PIPE_READMODE_MESSAGE;

const DWORD_ PIPE_WAIT_ = PIPE_WAIT;
const DWORD_ PIPE_NOWAIT_ = PIPE_NOWAIT;

const DWORD_ PIPE_UNLIMITED_INSTANCES_ = PIPE_UNLIMITED_INSTANCES;

const DWORD_ NMPWAIT_USE_DEFAULT_WAIT_ = NMPWAIT_USE_DEFAULT_WAIT;
const DWORD_ NMPWAIT_NOWAIT_ = NMPWAIT_NOWAIT;
const DWORD_ NMPWAIT_WAIT_FOREVER_ = NMPWAIT_WAIT_FOREVER;

#else // defined( AUTOBOOST_USE_WINDOWS_H )

const DWORD_ PIPE_ACCESS_DUPLEX_ = 0x00000003;
const DWORD_ PIPE_ACCESS_INBOUND_ = 0x00000001;
const DWORD_ PIPE_ACCESS_OUTBOUND_ = 0x00000002;

const DWORD_ PIPE_TYPE_BYTE_ = 0x00000000;
const DWORD_ PIPE_TYPE_MESSAGE_ = 0x00000004;

const DWORD_ PIPE_READMODE_BYTE_ = 0x00000000;
const DWORD_ PIPE_READMODE_MESSAGE_ = 0x00000002;

const DWORD_ PIPE_WAIT_ = 0x00000000;
const DWORD_ PIPE_NOWAIT_ = 0x00000001;

const DWORD_ PIPE_UNLIMITED_INSTANCES_ = 255u;

const DWORD_ NMPWAIT_USE_DEFAULT_WAIT_ = 0x00000000;
const DWORD_ NMPWAIT_NOWAIT_ = 0x00000001;
const DWORD_ NMPWAIT_WAIT_FOREVER_ = 0xFFFFFFFF;

#endif // defined( AUTOBOOST_USE_WINDOWS_H )

// These constants are not defined in Windows SDK prior to 7.0A
const DWORD_ PIPE_ACCEPT_REMOTE_CLIENTS_ = 0x00000000;
const DWORD_ PIPE_REJECT_REMOTE_CLIENTS_ = 0x00000008;

using ::ImpersonateNamedPipeClient;
using ::DisconnectNamedPipe;
using ::SetNamedPipeHandleState;
using ::PeekNamedPipe;

#if !defined( AUTOBOOST_NO_ANSI_APIS )
using ::WaitNamedPipeA;
#endif
using ::WaitNamedPipeW;

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6
#if !defined( AUTOBOOST_NO_ANSI_APIS )
using ::GetNamedPipeClientComputerNameA;
#endif // !defined( AUTOBOOST_NO_ANSI_APIS )
using ::GetNamedPipeClientComputerNameW;
#endif // AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6

AUTOBOOST_FORCEINLINE BOOL_ CreatePipe(PHANDLE_ hReadPipe, PHANDLE_ hWritePipe, LPSECURITY_ATTRIBUTES_ lpPipeAttributes, DWORD_ nSize)
{
    return ::CreatePipe(hReadPipe, hWritePipe, reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpPipeAttributes), nSize);
}

AUTOBOOST_FORCEINLINE BOOL_ ConnectNamedPipe(HANDLE_ hNamedPipe, LPOVERLAPPED_ lpOverlapped)
{
    return ::ConnectNamedPipe(hNamedPipe, reinterpret_cast< ::_OVERLAPPED* >(lpOverlapped));
}

AUTOBOOST_FORCEINLINE BOOL_ TransactNamedPipe(HANDLE_ hNamedPipe, LPVOID_ lpInBuffer, DWORD_ nInBufferSize, LPVOID_ lpOutBuffer, DWORD_ nOutBufferSize, LPDWORD_ lpBytesRead, LPOVERLAPPED_ lpOverlapped)
{
    return ::TransactNamedPipe(hNamedPipe, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, lpBytesRead, reinterpret_cast< ::_OVERLAPPED* >(lpOverlapped));
}


#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_FORCEINLINE HANDLE_ CreateNamedPipeA(
    LPCSTR_ lpName,
    DWORD_ dwOpenMode,
    DWORD_ dwPipeMode,
    DWORD_ nMaxInstances,
    DWORD_ nOutBufferSize,
    DWORD_ nInBufferSize,
    DWORD_ nDefaultTimeOut,
    LPSECURITY_ATTRIBUTES_ lpSecurityAttributes)
{
    return ::CreateNamedPipeA(
        lpName,
        dwOpenMode,
        dwPipeMode,
        nMaxInstances,
        nOutBufferSize,
        nInBufferSize,
        nDefaultTimeOut,
        reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpSecurityAttributes));
}

AUTOBOOST_FORCEINLINE HANDLE_ create_named_pipe(
    LPCSTR_ lpName,
    DWORD_ dwOpenMode,
    DWORD_ dwPipeMode,
    DWORD_ nMaxInstances,
    DWORD_ nOutBufferSize,
    DWORD_ nInBufferSize,
    DWORD_ nDefaultTimeOut,
    LPSECURITY_ATTRIBUTES_ lpSecurityAttributes)
{
    return ::CreateNamedPipeA(
        lpName,
        dwOpenMode,
        dwPipeMode,
        nMaxInstances,
        nOutBufferSize,
        nInBufferSize,
        nDefaultTimeOut,
        reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpSecurityAttributes));
}
#endif // !defined( AUTOBOOST_NO_ANSI_APIS )

AUTOBOOST_FORCEINLINE HANDLE_ CreateNamedPipeW(
    LPCWSTR_ lpName,
    DWORD_ dwOpenMode,
    DWORD_ dwPipeMode,
    DWORD_ nMaxInstances,
    DWORD_ nOutBufferSize,
    DWORD_ nInBufferSize,
    DWORD_ nDefaultTimeOut,
    LPSECURITY_ATTRIBUTES_ lpSecurityAttributes)
{
    return ::CreateNamedPipeW(
        lpName,
        dwOpenMode,
        dwPipeMode,
        nMaxInstances,
        nOutBufferSize,
        nInBufferSize,
        nDefaultTimeOut,
        reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpSecurityAttributes));
}

AUTOBOOST_FORCEINLINE HANDLE_ create_named_pipe(
    LPCWSTR_ lpName,
    DWORD_ dwOpenMode,
    DWORD_ dwPipeMode,
    DWORD_ nMaxInstances,
    DWORD_ nOutBufferSize,
    DWORD_ nInBufferSize,
    DWORD_ nDefaultTimeOut,
    LPSECURITY_ATTRIBUTES_ lpSecurityAttributes)
{
    return ::CreateNamedPipeW(
        lpName,
        dwOpenMode,
        dwPipeMode,
        nMaxInstances,
        nOutBufferSize,
        nInBufferSize,
        nDefaultTimeOut,
        reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpSecurityAttributes));
}

#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_FORCEINLINE BOOL_ wait_named_pipe(LPCSTR_ lpNamedPipeName, DWORD_ nTimeOut)
{
    return ::WaitNamedPipeA(lpNamedPipeName, nTimeOut);
}
#endif //AUTOBOOST_NO_ANSI_APIS

AUTOBOOST_FORCEINLINE BOOL_ wait_named_pipe(LPCWSTR_ lpNamedPipeName, DWORD_ nTimeOut)
{
    return ::WaitNamedPipeW(lpNamedPipeName, nTimeOut);
}

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6

#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_FORCEINLINE BOOL_ get_named_pipe_client_computer_name(HANDLE_ Pipe, LPSTR_ ClientComputerName, ULONG_ ClientComputerNameLength)
{
    return ::GetNamedPipeClientComputerNameA(Pipe, ClientComputerName, ClientComputerNameLength);
}
#endif // !defined( AUTOBOOST_NO_ANSI_APIS )

AUTOBOOST_FORCEINLINE BOOL_ get_named_pipe_client_computer_name(HANDLE_ Pipe, LPWSTR_ ClientComputerName, ULONG_ ClientComputerNameLength)
{
    return ::GetNamedPipeClientComputerNameW(Pipe, ClientComputerName, ClientComputerNameLength);
}

#endif // AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WIN6

}
}
}

#endif // AUTOBOOST_PLAT_WINDOWS_DESKTOP

#endif // AUTOBOOST_DETAIL_WINAPI_PIPES_HPP_
