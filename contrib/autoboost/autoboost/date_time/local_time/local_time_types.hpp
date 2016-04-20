#ifndef AB_LOCAL_TIME_LOCAL_TIME_TYPES_HPP__
#define AB_LOCAL_TIME_LOCAL_TIME_TYPES_HPP__

/* Copyright (c) 2003-2004 CrystalClear Software, Inc.
 * Subject to the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)
 * Author: Jeff Garland, Bart Garst
 * $Date$
 */

#include "autoboost/date_time/local_time/local_date_time.hpp"
#include "autoboost/date_time/period.hpp"
#include "autoboost/date_time/time_iterator.hpp"
#include "autoboost/date_time/compiler_config.hpp"
#if defined(AUTOBOOST_DATE_TIME_OPTIONAL_GREGORIAN_TYPES)
#include "autoboost/date_time/local_time/date_duration_operators.hpp"
#endif //AUTOBOOST_DATE_TIME_OPTIONAL_GREGORIAN_TYPES
#include "autoboost/date_time/local_time/custom_time_zone.hpp"

namespace autoboost {
namespace local_time {

  typedef autoboost::date_time::period<local_date_time,
                                   autoboost::posix_time::time_duration> local_time_period;

  typedef date_time::time_itr<local_date_time> local_time_iterator;

  typedef date_time::second_clock<local_date_time> local_sec_clock;
  typedef date_time::microsec_clock<local_date_time> local_microsec_clock;

  typedef date_time::time_zone_base<posix_time::ptime, char> time_zone;
  typedef date_time::time_zone_base<posix_time::ptime, wchar_t> wtime_zone;

  //! Shared Pointer for custom_time_zone and posix_time_zone objects
  typedef autoboost::shared_ptr<time_zone> time_zone_ptr;
  typedef autoboost::shared_ptr<wtime_zone> wtime_zone_ptr;

  typedef date_time::time_zone_names_base<char> time_zone_names;
  typedef date_time::time_zone_names_base<wchar_t> wtime_zone_names;

  //bring special enum values into the namespace
  using date_time::special_values;
  using date_time::not_special;
  using date_time::neg_infin;
  using date_time::pos_infin;
  using date_time::not_a_date_time;
  using date_time::max_date_time;
  using date_time::min_date_time;

}} // namespaces

#endif // LOCAL_TIME_LOCAL_TIME_TYPES_HPP__
