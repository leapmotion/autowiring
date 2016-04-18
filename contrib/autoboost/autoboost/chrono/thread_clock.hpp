//  autoboost/chrono/thread_clock.hpp  -----------------------------------------------------------//

//  Copyright 2009-2011 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  See http://www.boost.org/libs/system for documentation.

#include <autoboost/chrono/config.hpp>

#ifndef AUTOBOOST_CHRONO_THREAD_CLOCK_HPP
#define AUTOBOOST_CHRONO_THREAD_CLOCK_HPP

#if defined(AUTOBOOST_CHRONO_HAS_THREAD_CLOCK)

#include <autoboost/chrono/config.hpp>
#include <autoboost/chrono/duration.hpp>
#include <autoboost/chrono/time_point.hpp>
#include <autoboost/chrono/detail/system.hpp>
#include <autoboost/chrono/clock_string.hpp>

#ifndef AUTOBOOST_CHRONO_HEADER_ONLY
#include <autoboost/config/abi_prefix.hpp> // must be the last #include
#endif

namespace autoboost { namespace chrono {

class AUTOBOOST_CHRONO_DECL thread_clock {
public:
    typedef nanoseconds                          duration;
    typedef duration::rep                        rep;
    typedef duration::period                     period;
    typedef chrono::time_point<thread_clock>    time_point;
    AUTOBOOST_STATIC_CONSTEXPR bool is_steady =             AUTOBOOST_CHRONO_THREAD_CLOCK_IS_STEADY;

    static AUTOBOOST_CHRONO_INLINE time_point now( ) AUTOBOOST_NOEXCEPT;
#if !defined AUTOBOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING
    static AUTOBOOST_CHRONO_INLINE time_point now( system::error_code & ec );
#endif
};

template <class CharT>
struct clock_string<thread_clock, CharT>
{
  static std::basic_string<CharT> name()
  {
    static const CharT u[] =
    { 't', 'h', 'r', 'e', 'a', 'd', '_',
      'c', 'l','o', 'c', 'k'};
    static const std::basic_string<CharT> str(u, u + sizeof(u)/sizeof(u[0]));
    return str;
  }
  static std::basic_string<CharT> since()
  {
    const CharT u[] =
    { ' ', 's', 'i', 'n', 'c', 'e', ' ', 't', 'h', 'r', 'e', 'a', 'd', ' ', 's', 't', 'a', 'r', 't', '-', 'u', 'p'};
    const std::basic_string<CharT> str(u, u + sizeof(u)/sizeof(u[0]));
    return str;
  }
};

} // namespace chrono
} // namespace autoboost


#ifndef AUTOBOOST_CHRONO_HEADER_ONLY
#include <autoboost/config/abi_suffix.hpp> // pops abi_prefix.hpp pragmas
#else
#include <autoboost/chrono/detail/inlined/thread_clock.hpp>
#endif

#endif

#endif  // AUTOBOOST_CHRONO_THREAD_CLOCK_HPP
