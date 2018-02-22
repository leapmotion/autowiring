//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTERPROCESS_POSIX_TIMES_WRK_HPP
#define AUTOBOOST_INTERPROCESS_POSIX_TIMES_WRK_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

//workaround to avoid winsock redefines when using date-time

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define AUTOBOOST_INTERPROCESS_WIN32_LEAN_AND_MEAN
#endif   //#ifndef WIN32_LEAN_AND_MEAN
#endif   //#ifdef _WIN32

#include <autoboost/date_time/microsec_time_clock.hpp>
#include <autoboost/date_time/posix_time/ptime.hpp>
#include <autoboost/date_time/posix_time/posix_time_duration.hpp>

namespace autoboost {
namespace interprocess {

typedef autoboost::date_time::microsec_clock<autoboost::posix_time::ptime> microsec_clock;

}
}

#ifdef _WIN32
#ifdef AUTOBOOST_INTERPROCESS_WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#undef AUTOBOOST_INTERPROCESS_WIN32_LEAN_AND_MEAN
#endif   //#ifdef AUTOBOOST_INTERPROCESS_WIN32_LEAN_AND_MEAN
#endif   //#ifdef _WIN32

#endif   //#ifndef AUTOBOOST_INTERPROCESS_POSIX_TIMES_WRK_HPP

