//  autoboost process_timer.cpp  -----------------------------------------------------------//

//  Copyright Beman Dawes 1994, 2006, 2008
//  Copyright 2009-2010 Vicente J. Botet Escriba
//  Copyright (c) Microsoft Corporation 2014

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  See http://www.boost.org/libs/chrono for documentation.

//--------------------------------------------------------------------------------------//
#ifndef AUTOBOOST_CHRONO_DETAIL_INLINED_WIN_PROCESS_CLOCK_HPP
#define AUTOBOOST_CHRONO_DETAIL_INLINED_WIN_PROCESS_CLOCK_HPP

#include <autoboost/chrono/config.hpp>
//#include <autoboost/chrono/system_clocks.hpp>
#include <autoboost/chrono/process_cpu_clocks.hpp>
#include <cassert>
#include <time.h>

#include <autoboost/detail/winapi/GetLastError.hpp>
#include <autoboost/detail/winapi/GetCurrentProcess.hpp>
#if AUTOBOOST_PLAT_WINDOWS_DESKTOP
#include <autoboost/detail/winapi/GetProcessTimes.hpp>
#endif

namespace autoboost
{
namespace chrono
{

process_real_cpu_clock::time_point process_real_cpu_clock::now() AUTOBOOST_NOEXCEPT
{
    clock_t c = ::clock();
    if ( c == clock_t(-1) ) // error
    {
      AUTOBOOST_ASSERT(0 && "Boost::Chrono - Internal Error");
    }
    typedef ratio_divide<giga, ratio<CLOCKS_PER_SEC> >::type R;
    return time_point(
      duration(static_cast<rep>(c)*R::num/R::den)
    );
}

#if !defined AUTOBOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING
process_real_cpu_clock::time_point process_real_cpu_clock::now(
        system::error_code & ec)
{
    clock_t c = ::clock();
    if ( c == clock_t(-1) ) // error
    {
            autoboost::throw_exception(
                    system::system_error(
                            errno,
                            AUTOBOOST_CHRONO_SYSTEM_CATEGORY,
                            "chrono::process_real_cpu_clock" ));
    }
    if (!AUTOBOOST_CHRONO_IS_THROWS(ec))
    {
      ec.clear();
    }
    typedef ratio_divide<giga, ratio<CLOCKS_PER_SEC> >::type R;
    return time_point(
      duration(static_cast<rep>(c)*R::num/R::den)
    );
}
#endif

#if AUTOBOOST_PLAT_WINDOWS_DESKTOP
process_user_cpu_clock::time_point process_user_cpu_clock::now() AUTOBOOST_NOEXCEPT
{

    //  note that Windows uses 100 nanosecond ticks for FILETIME
    autoboost::detail::winapi::FILETIME_ creation, exit, user_time, system_time;

    if ( autoboost::detail::winapi::GetProcessTimes(
            autoboost::detail::winapi::GetCurrentProcess(), &creation, &exit,
            &system_time, &user_time ) )
    {
        return time_point(duration(
                ((static_cast<process_user_cpu_clock::rep>(user_time.dwHighDateTime) << 32)
                  | user_time.dwLowDateTime) * 100
                ));
    }
    else
    {
        AUTOBOOST_ASSERT(0 && "Boost::Chrono - Internal Error");
        return time_point();
    }

}

#if !defined AUTOBOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING
process_user_cpu_clock::time_point process_user_cpu_clock::now(
        system::error_code & ec)
{

    //  note that Windows uses 100 nanosecond ticks for FILETIME
    autoboost::detail::winapi::FILETIME_ creation, exit, user_time, system_time;

    if ( autoboost::detail::winapi::GetProcessTimes(
            autoboost::detail::winapi::GetCurrentProcess(), &creation, &exit,
            &system_time, &user_time ) )
    {
        if (!AUTOBOOST_CHRONO_IS_THROWS(ec))
        {
            ec.clear();
        }
        return time_point(duration(
                ((static_cast<process_user_cpu_clock::rep>(user_time.dwHighDateTime) << 32)
                  | user_time.dwLowDateTime) * 100
                ));
    }
    else
    {
        autoboost::detail::winapi::DWORD_ cause = autoboost::detail::winapi::GetLastError();
        if (AUTOBOOST_CHRONO_IS_THROWS(ec))
        {
            autoboost::throw_exception(
                    system::system_error(
                            cause,
                            AUTOBOOST_CHRONO_SYSTEM_CATEGORY,
                            "chrono::process_user_cpu_clock" ));
        }
        else
        {
            ec.assign( cause, AUTOBOOST_CHRONO_SYSTEM_CATEGORY );
            return time_point();
        }
    }

}
#endif

process_system_cpu_clock::time_point process_system_cpu_clock::now() AUTOBOOST_NOEXCEPT
{

    //  note that Windows uses 100 nanosecond ticks for FILETIME
    autoboost::detail::winapi::FILETIME_ creation, exit, user_time, system_time;

    if ( autoboost::detail::winapi::GetProcessTimes(
            autoboost::detail::winapi::GetCurrentProcess(), &creation, &exit,
            &system_time, &user_time ) )
    {
        return time_point(duration(
                ((static_cast<process_system_cpu_clock::rep>(system_time.dwHighDateTime) << 32)
                                    | system_time.dwLowDateTime) * 100
                ));
    }
    else
    {
      AUTOBOOST_ASSERT(0 && "Boost::Chrono - Internal Error");
      return time_point();
    }

}

#if !defined AUTOBOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING
process_system_cpu_clock::time_point process_system_cpu_clock::now(
        system::error_code & ec)
{

    //  note that Windows uses 100 nanosecond ticks for FILETIME
    autoboost::detail::winapi::FILETIME_ creation, exit, user_time, system_time;

    if ( autoboost::detail::winapi::GetProcessTimes(
            autoboost::detail::winapi::GetCurrentProcess(), &creation, &exit,
            &system_time, &user_time ) )
    {
        if (!AUTOBOOST_CHRONO_IS_THROWS(ec))
        {
            ec.clear();
        }
        return time_point(duration(
                ((static_cast<process_system_cpu_clock::rep>(system_time.dwHighDateTime) << 32)
                                    | system_time.dwLowDateTime) * 100
                ));
    }
    else
    {
        autoboost::detail::winapi::DWORD_ cause = autoboost::detail::winapi::GetLastError();
        if (AUTOBOOST_CHRONO_IS_THROWS(ec))
        {
            autoboost::throw_exception(
                    system::system_error(
                            cause,
                            AUTOBOOST_CHRONO_SYSTEM_CATEGORY,
                            "chrono::process_system_cpu_clock" ));
        }
        else
        {
            ec.assign( cause, AUTOBOOST_CHRONO_SYSTEM_CATEGORY );
            return time_point();
        }
    }

}
#endif

process_cpu_clock::time_point process_cpu_clock::now()  AUTOBOOST_NOEXCEPT
{

    //  note that Windows uses 100 nanosecond ticks for FILETIME
    autoboost::detail::winapi::FILETIME_ creation, exit, user_time, system_time;

    if ( autoboost::detail::winapi::GetProcessTimes(
            autoboost::detail::winapi::GetCurrentProcess(), &creation, &exit,
            &system_time, &user_time ) )
    {
        time_point::rep r(process_real_cpu_clock::now().time_since_epoch().count()
                            ,
                ((static_cast<process_user_cpu_clock::rep>(user_time.dwHighDateTime) << 32)
                        | user_time.dwLowDateTime
                ) * 100,
                ((static_cast<process_system_cpu_clock::rep>(system_time.dwHighDateTime) << 32)
                        | system_time.dwLowDateTime
                ) * 100
        );
        return time_point(duration(r));
    }
    else
    {
      AUTOBOOST_ASSERT(0 && "Boost::Chrono - Internal Error");
      return time_point();
    }

}

#if !defined AUTOBOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING
process_cpu_clock::time_point process_cpu_clock::now(
        system::error_code & ec )
{

    //  note that Windows uses 100 nanosecond ticks for FILETIME
    autoboost::detail::winapi::FILETIME_ creation, exit, user_time, system_time;

    if ( autoboost::detail::winapi::GetProcessTimes(
            autoboost::detail::winapi::GetCurrentProcess(), &creation, &exit,
            &system_time, &user_time ) )
    {
        if (!AUTOBOOST_CHRONO_IS_THROWS(ec))
        {
            ec.clear();
        }
        time_point::rep r(process_real_cpu_clock::now().time_since_epoch().count()
                            ,
                ((static_cast<process_user_cpu_clock::rep>(user_time.dwHighDateTime) << 32)
                        | user_time.dwLowDateTime
                ) * 100,
                ((static_cast<process_system_cpu_clock::rep>(system_time.dwHighDateTime) << 32)
                        | system_time.dwLowDateTime
                ) * 100
        );
        return time_point(duration(r));
    }
    else
    {
        autoboost::detail::winapi::DWORD_ cause = autoboost::detail::winapi::GetLastError();
        if (AUTOBOOST_CHRONO_IS_THROWS(ec))
        {
            autoboost::throw_exception(
                    system::system_error(
                            cause,
                            AUTOBOOST_CHRONO_SYSTEM_CATEGORY,
                            "chrono::process_cpu_clock" ));
        }
        else
        {
            ec.assign( cause, AUTOBOOST_CHRONO_SYSTEM_CATEGORY );
            return time_point();
        }
    }

}
#endif
#endif
} // namespace chrono
} // namespace autoboost

#endif
