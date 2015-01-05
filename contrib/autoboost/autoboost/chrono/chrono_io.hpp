
//  chrono_io
//
//  (C) Copyright Howard Hinnant
//  (C) Copyright 2010-2011 Vicente J. Botet Escriba
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
// This code was adapted by Vicente from Howard Hinnant's experimental work
// on chrono i/o under lvm/libc++  to Boost

#ifndef AUTOBOOST_CHRONO_CHRONO_IO_HPP
#define AUTOBOOST_CHRONO_CHRONO_IO_HPP

#include <autoboost/chrono/config.hpp>

//#if AUTOBOOST_CHRONO_VERSION == 2
//#include <autoboost/chrono/io/time_point_io.hpp>
//#include <autoboost/chrono/io/duration_io.hpp>
//#elif AUTOBOOST_CHRONO_VERSION == 1
//#include <autoboost/chrono/io_v1/chrono_io.hpp>
//#endif

#if defined AUTOBOOST_CHRONO_DONT_PROVIDES_DEPRECATED_IO_SINCE_V2_0_0
#include <autoboost/chrono/io/time_point_io.hpp>
#include <autoboost/chrono/io/duration_io.hpp>
#else
#include <autoboost/chrono/io_v1/chrono_io.hpp>
#endif

#include <autoboost/chrono/io/utility/to_string.hpp>

#endif  // AUTOBOOST_CHRONO_CHRONO_IO_HPP
