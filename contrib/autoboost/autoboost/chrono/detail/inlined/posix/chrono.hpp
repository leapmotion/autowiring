//  posix/chrono.cpp  --------------------------------------------------------------//

//  Copyright Beman Dawes 2008
//  Copyright Vicente J. Botet Escriba 2009

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//----------------------------------------------------------------------------//
//                                POSIX                                     //
//----------------------------------------------------------------------------//

#include <time.h>  // for clock_gettime
#include <autoboost/assert.hpp>

namespace autoboost
{
namespace chrono
{

  system_clock::time_point system_clock::now() AUTOBOOST_NOEXCEPT
  {
    timespec ts;
    if ( ::clock_gettime( CLOCK_REALTIME, &ts ) )
    {
      AUTOBOOST_ASSERT(0 && "Boost::Chrono - Internal Error");
    }

    return time_point(duration(
      static_cast<system_clock::rep>( ts.tv_sec ) * 1000000000 + ts.tv_nsec));
  }

#if !defined AUTOBOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING
  system_clock::time_point system_clock::now(system::error_code & ec)
  {
    timespec ts;
    if ( ::clock_gettime( CLOCK_REALTIME, &ts ) )
    {
        if (AUTOBOOST_CHRONO_IS_THROWS(ec))
        {
            autoboost::throw_exception(
                    system::system_error(
                            errno,
                            AUTOBOOST_CHRONO_SYSTEM_CATEGORY,
                            "chrono::system_clock" ));
        }
        else
        {
            ec.assign( errno, AUTOBOOST_CHRONO_SYSTEM_CATEGORY );
            return time_point();
        }
    }

    if (!AUTOBOOST_CHRONO_IS_THROWS(ec))
    {
        ec.clear();
    }
    return time_point(duration(
      static_cast<system_clock::rep>( ts.tv_sec ) * 1000000000 + ts.tv_nsec));
  }
#endif

  std::time_t system_clock::to_time_t(const system_clock::time_point& t) AUTOBOOST_NOEXCEPT
  {
      return static_cast<std::time_t>( t.time_since_epoch().count() / 1000000000 );
  }

  system_clock::time_point system_clock::from_time_t(std::time_t t) AUTOBOOST_NOEXCEPT
  {
      return time_point(duration(static_cast<system_clock::rep>(t) * 1000000000));
  }

#ifdef AUTOBOOST_CHRONO_HAS_CLOCK_STEADY

  steady_clock::time_point steady_clock::now() AUTOBOOST_NOEXCEPT
  {
    timespec ts;
    if ( ::clock_gettime( CLOCK_MONOTONIC, &ts ) )
    {
      AUTOBOOST_ASSERT(0 && "Boost::Chrono - Internal Error");
    }

    return time_point(duration(
      static_cast<steady_clock::rep>( ts.tv_sec ) * 1000000000 + ts.tv_nsec));
  }

#if !defined AUTOBOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING
  steady_clock::time_point steady_clock::now(system::error_code & ec)
  {
    timespec ts;
    if ( ::clock_gettime( CLOCK_MONOTONIC, &ts ) )
    {
        if (AUTOBOOST_CHRONO_IS_THROWS(ec))
        {
            autoboost::throw_exception(
                    system::system_error(
                            errno,
                            AUTOBOOST_CHRONO_SYSTEM_CATEGORY,
                            "chrono::steady_clock" ));
        }
        else
        {
            ec.assign( errno, AUTOBOOST_CHRONO_SYSTEM_CATEGORY );
            return time_point();
        }
    }

    if (!AUTOBOOST_CHRONO_IS_THROWS(ec))
    {
        ec.clear();
    }
    return time_point(duration(
      static_cast<steady_clock::rep>( ts.tv_sec ) * 1000000000 + ts.tv_nsec));
  }
#endif
#endif

}  // namespace chrono
}  // namespace autoboost


