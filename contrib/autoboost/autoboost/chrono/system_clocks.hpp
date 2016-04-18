//  autoboost/chrono/system_clocks.hpp  --------------------------------------------------------------//

//  Copyright 2008 Howard Hinnant
//  Copyright 2008 Beman Dawes
//  Copyright 2009-2011 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

/*

This code was derived by Beman Dawes from Howard Hinnant's time2_demo prototype.
Many thanks to Howard for making his code available under the Boost license.
The original code was modified to conform to Boost conventions and to section
20.9 Time utilities [time] of the C++ committee's working paper N2798.
See http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2798.pdf.

time2_demo contained this comment:

    Much thanks to Andrei Alexandrescu,
                   Walter Brown,
                   Peter Dimov,
                   Jeff Garland,
                   Terry Golubiewski,
                   Daniel Krugler,
                   Anthony Williams.
*/

/*

TODO:

  * Fully implement error handling, with test cases.
  * Consider issues raised by Michael Marcin:

    > In the past I've seen QueryPerformanceCounter give incorrect results,
    > especially with SpeedStep processors on laptops. This was many years ago and
    > might have been fixed by service packs and drivers.
    >
    > Typically you check the results of QPC against GetTickCount to see if the
    > results are reasonable.
    > http://support.microsoft.com/kb/274323
    >
    > I've also heard of problems with QueryPerformanceCounter in multi-processor
    > systems.
    >
    > I know some people SetThreadAffinityMask to 1 for the current thread call
    > their QueryPerformance* functions then restore SetThreadAffinityMask. This
    > seems horrible to me because it forces your program to jump to another
    > physical processor if it isn't already on cpu0 but they claim it worked well
    > in practice because they called the timing functions infrequently.
    >
    > In the past I have chosen to use timeGetTime with timeBeginPeriod(1) for
    > high resolution timers to avoid these issues.

*/

#ifndef AUTOBOOST_CHRONO_SYSTEM_CLOCKS_HPP
#define AUTOBOOST_CHRONO_SYSTEM_CLOCKS_HPP

#include <autoboost/chrono/config.hpp>
#include <autoboost/chrono/duration.hpp>
#include <autoboost/chrono/time_point.hpp>
#include <autoboost/chrono/detail/system.hpp>
#include <autoboost/chrono/clock_string.hpp>

#include <ctime>

# if defined( AUTOBOOST_CHRONO_POSIX_API )
#   if ! defined(CLOCK_REALTIME) && ! defined (__hpux__)
#     error <time.h> does not supply CLOCK_REALTIME
#   endif
# endif

#ifdef AUTOBOOST_CHRONO_WINDOWS_API
// The system_clock tick is 100 nanoseconds
# define AUTOBOOST_SYSTEM_CLOCK_DURATION autoboost::chrono::duration<autoboost::int_least64_t, ratio<AUTOBOOST_RATIO_INTMAX_C(1), AUTOBOOST_RATIO_INTMAX_C(10000000)> >
#else
# define AUTOBOOST_SYSTEM_CLOCK_DURATION autoboost::chrono::nanoseconds
#endif

// this must occur after all of the includes and before any code appears:
#ifndef AUTOBOOST_CHRONO_HEADER_ONLY
#include <autoboost/config/abi_prefix.hpp> // must be the last #include
#endif


//----------------------------------------------------------------------------//
//                                                                            //
//                        20.9 Time utilities [time]                          //
//                                 synopsis                                   //
//                                                                            //
//----------------------------------------------------------------------------//

namespace autoboost {
namespace chrono {

  // Clocks
  class AUTOBOOST_CHRONO_DECL system_clock;
#ifdef AUTOBOOST_CHRONO_HAS_CLOCK_STEADY
  class AUTOBOOST_CHRONO_DECL steady_clock;
#endif

#ifdef AUTOBOOST_CHRONO_HAS_CLOCK_STEADY
  typedef steady_clock high_resolution_clock;  // as permitted by [time.clock.hires]
#else
  typedef system_clock high_resolution_clock;  // as permitted by [time.clock.hires]
#endif

//----------------------------------------------------------------------------//
//                                                                            //
//      20.9.5 Clocks [time.clock]                                            //
//                                                                            //
//----------------------------------------------------------------------------//

// If you're porting, clocks are the system-specific (non-portable) part.
// You'll need to know how to get the current time and implement that under now().
// You'll need to know what units (tick period) and representation makes the most
// sense for your clock and set those accordingly.
// If you know how to map this clock to time_t (perhaps your clock is std::time, which
// makes that trivial), then you can fill out system_clock's to_time_t() and from_time_t().

//----------------------------------------------------------------------------//
//      20.9.5.1 Class system_clock [time.clock.system]                       //
//----------------------------------------------------------------------------//

  class AUTOBOOST_CHRONO_DECL system_clock
  {
  public:
      typedef AUTOBOOST_SYSTEM_CLOCK_DURATION          duration;
      typedef duration::rep                        rep;
      typedef duration::period                     period;
      typedef chrono::time_point<system_clock>     time_point;
      AUTOBOOST_STATIC_CONSTEXPR bool is_steady =             false;

      static AUTOBOOST_CHRONO_INLINE time_point  now() AUTOBOOST_NOEXCEPT;
#if !defined AUTOBOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING
      static AUTOBOOST_CHRONO_INLINE time_point  now(system::error_code & ec);
#endif

      static AUTOBOOST_CHRONO_INLINE std::time_t to_time_t(const time_point& t) AUTOBOOST_NOEXCEPT;
      static AUTOBOOST_CHRONO_INLINE time_point  from_time_t(std::time_t t) AUTOBOOST_NOEXCEPT;
  };

//----------------------------------------------------------------------------//
//      20.9.5.2 Class steady_clock [time.clock.steady]                 //
//----------------------------------------------------------------------------//

// As permitted  by [time.clock.steady]
// The class steady_clock is conditionally supported.

#ifdef AUTOBOOST_CHRONO_HAS_CLOCK_STEADY
  class AUTOBOOST_CHRONO_DECL steady_clock
  {
  public:
      typedef nanoseconds                          duration;
      typedef duration::rep                        rep;
      typedef duration::period                     period;
      typedef chrono::time_point<steady_clock>  time_point;
      AUTOBOOST_STATIC_CONSTEXPR bool is_steady =             true;

      static AUTOBOOST_CHRONO_INLINE time_point  now() AUTOBOOST_NOEXCEPT;
#if !defined AUTOBOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING
      static AUTOBOOST_CHRONO_INLINE time_point  now(system::error_code & ec);
#endif
  };
#endif
//----------------------------------------------------------------------------//
//      20.9.5.3 Class high_resolution_clock [time.clock.hires]               //
//----------------------------------------------------------------------------//

//  As permitted, steady_clock or system_clock is a typedef for high_resolution_clock.
//  See synopsis.


  template<class CharT>
  struct clock_string<system_clock, CharT>
  {
    static std::basic_string<CharT> name()
    {
      static const CharT u[] =
      { 's', 'y', 's', 't', 'e', 'm', '_', 'c', 'l', 'o', 'c', 'k' };
      static const std::basic_string<CharT> str(u, u + sizeof(u)
          / sizeof(u[0]));
      return str;
    }
    static std::basic_string<CharT> since()
    {
      static const CharT
          u[] =
              { ' ', 's', 'i', 'n', 'c', 'e', ' ', 'J', 'a', 'n', ' ', '1', ',', ' ', '1', '9', '7', '0' };
      static const std::basic_string<CharT> str(u, u + sizeof(u)
          / sizeof(u[0]));
      return str;
    }
  };

#ifdef AUTOBOOST_CHRONO_HAS_CLOCK_STEADY

  template<class CharT>
  struct clock_string<steady_clock, CharT>
  {
    static std::basic_string<CharT> name()
    {
      static const CharT
          u[] =
              { 's', 't', 'e', 'a', 'd', 'y', '_', 'c', 'l', 'o', 'c', 'k' };
      static const std::basic_string<CharT> str(u, u + sizeof(u)
          / sizeof(u[0]));
      return str;
    }
    static std::basic_string<CharT> since()
    {
      const CharT u[] =
      { ' ', 's', 'i', 'n', 'c', 'e', ' ', 'b', 'o', 'o', 't' };
      const std::basic_string<CharT> str(u, u + sizeof(u) / sizeof(u[0]));
      return str;
    }
  };

#endif

} // namespace chrono
} // namespace autoboost

#ifndef AUTOBOOST_CHRONO_HEADER_ONLY
// the suffix header occurs after all of our code:
#include <autoboost/config/abi_suffix.hpp> // pops abi_prefix.hpp pragmas
#else
#include <autoboost/chrono/detail/inlined/chrono.hpp>
#endif

#endif // AUTOBOOST_CHRONO_SYSTEM_CLOCKS_HPP
