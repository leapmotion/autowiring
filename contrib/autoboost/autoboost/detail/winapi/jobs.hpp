//  jobs.hpp  --------------------------------------------------------------//

//  Copyright 2016 Klemens D. Morgenstern

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#ifndef AUTOBOOST_DETAIL_WINAPI_JOBS_HPP_
#define AUTOBOOST_DETAIL_WINAPI_JOBS_HPP_

#include <autoboost/detail/winapi/basic_types.hpp>
#include <autoboost/detail/winapi/access_rights.hpp>

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI CreateJobObjectA(
    ::_SECURITY_ATTRIBUTES* lpJobAttributes,
    autoboost::detail::winapi::LPCSTR_ lpName);
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI CreateJobObjectW(
    ::_SECURITY_ATTRIBUTES* lpJobAttributes,
    autoboost::detail::winapi::LPCWSTR_ lpName);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI AssignProcessToJobObject(
    autoboost::detail::winapi::HANDLE_ hJob,
    autoboost::detail::winapi::HANDLE_ hProcess);

#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WINXP
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI IsProcessInJob(
    autoboost::detail::winapi::HANDLE_ ProcessHandle,
    autoboost::detail::winapi::HANDLE_ JobHandle,
    autoboost::detail::winapi::PBOOL_ Result);
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI TerminateJobObject(
    autoboost::detail::winapi::HANDLE_ hJob,
    autoboost::detail::winapi::UINT_ uExitCode);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI OpenJobObjectA(
    autoboost::detail::winapi::DWORD_ dwDesiredAccess,
    autoboost::detail::winapi::BOOL_ bInheritHandles,
    autoboost::detail::winapi::LPCSTR_ lpName);

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::HANDLE_ WINAPI OpenJobObjectW(
    autoboost::detail::winapi::DWORD_ dwDesiredAccess,
    autoboost::detail::winapi::BOOL_ bInheritHandles,
    autoboost::detail::winapi::LPCWSTR_ lpName);
} // extern "C"
#endif // !defined( AUTOBOOST_USE_WINDOWS_H )

namespace autoboost { namespace detail { namespace winapi {

#if defined( AUTOBOOST_USE_WINDOWS_H )
const DWORD_ JOB_OBJECT_ASSIGN_PROCESS_ = JOB_OBJECT_ASSIGN_PROCESS;
const DWORD_ JOB_OBJECT_SET_ATTRIBUTES_ = JOB_OBJECT_SET_ATTRIBUTES;
const DWORD_ JOB_OBJECT_QUERY_ = JOB_OBJECT_QUERY;
const DWORD_ JOB_OBJECT_TERMINATE_ = JOB_OBJECT_TERMINATE;
const DWORD_ JOB_OBJECT_SET_SECURITY_ATTRIBUTES_ = JOB_OBJECT_SET_SECURITY_ATTRIBUTES;
const DWORD_ JOB_OBJECT_ALL_ACCESS_ = JOB_OBJECT_ALL_ACCESS;
#else
const DWORD_ JOB_OBJECT_ASSIGN_PROCESS_ = 0x0001;
const DWORD_ JOB_OBJECT_SET_ATTRIBUTES_ = 0x0002;
const DWORD_ JOB_OBJECT_QUERY_ = 0x0004;
const DWORD_ JOB_OBJECT_TERMINATE_ = 0x0008;
const DWORD_ JOB_OBJECT_SET_SECURITY_ATTRIBUTES_ = 0x0010;
const DWORD_ JOB_OBJECT_ALL_ACCESS_ = (STANDARD_RIGHTS_REQUIRED_ | SYNCHRONIZE_ | 0x1F);
#endif

#if !defined( AUTOBOOST_NO_ANSI_APIS )
using ::OpenJobObjectA;
#endif

using ::OpenJobObjectW;

using ::AssignProcessToJobObject;
#if AUTOBOOST_USE_WINAPI_VERSION >= AUTOBOOST_WINAPI_VERSION_WINXP
using ::IsProcessInJob;
#endif
using ::TerminateJobObject;

#if !defined( AUTOBOOST_NO_ANSI_APIS )
AUTOBOOST_FORCEINLINE HANDLE_ CreateJobObjectA(LPSECURITY_ATTRIBUTES_ lpJobAttributes, LPCSTR_ lpName)
{
    return ::CreateJobObjectA(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpJobAttributes), lpName);
}

AUTOBOOST_FORCEINLINE HANDLE_ create_job_object(LPSECURITY_ATTRIBUTES_ lpJobAttributes, LPCSTR_ lpName)
{
    return ::CreateJobObjectA(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpJobAttributes), lpName);
}

AUTOBOOST_FORCEINLINE HANDLE_ open_job_object(DWORD_ dwDesiredAccess, BOOL_ bInheritHandles, LPCSTR_ lpName)
{
    return ::OpenJobObjectA(dwDesiredAccess, bInheritHandles, lpName);
}
#endif // !defined( AUTOBOOST_NO_ANSI_APIS )

AUTOBOOST_FORCEINLINE HANDLE_ CreateJobObjectW(LPSECURITY_ATTRIBUTES_ lpJobAttributes, LPCWSTR_ lpName)
{
    return ::CreateJobObjectW(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpJobAttributes), lpName);
}

AUTOBOOST_FORCEINLINE HANDLE_ create_job_object(LPSECURITY_ATTRIBUTES_ lpJobAttributes, LPCWSTR_ lpName)
{
    return ::CreateJobObjectW(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpJobAttributes), lpName);
}

AUTOBOOST_FORCEINLINE HANDLE_ open_job_object(DWORD_ dwDesiredAccess, BOOL_ bInheritHandles, LPCWSTR_ lpName)
{
    return OpenJobObjectW(dwDesiredAccess, bInheritHandles, lpName);
}

} // namespace winapi
} // namespace detail
} // namespace autoboost

#endif // AUTOBOOST_DETAIL_WINAPI_JOBS_HPP_
